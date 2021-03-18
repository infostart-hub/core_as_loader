/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Вход в приложение
*/

#include "pch.h"
#include "starter.h"
#include "modules_list.h"

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
HWND hMainWnd;
// Сюда будем писать команды для x64 инжектора - снять хук/установить хук
HANDLE h64Write = nullptr;
const wchar_t szWindowClass[] = L"Core.As.Starter";

/*
* Назначение программы:
* В фоновом режиме внедрять в нужные процессы ядро core_as с указанными модулями
* При старте программа сканирует каталог load и обрабатывает все txt файлы из него
* В файлах описывается, какие модули к каким процессам подключать
* 
* Модуль - папка в каталоге modules
* может содержать файл module.info
*   [info]
*    descr = Название модуля
*    src - список папок, с файлами *.as - исходниками модуля, пути относительно папки модуля.
*    api - список папок, с файлами *.v - описания подключаемого внешнего API модуля, пути относительно папки модуля.
*    lib - список загружаемых dll, пути относительно папки lib\[x86|x64] в папке модуля
* любые произвольные файлы и папки
* 
* Подготовленый список подключаемых к процессам модулей размещается в shared memory
* После этого устанавливается глобальный хук WH_CBT
* Обработчик хука находится в x86\inject.dll, которая будет загружаться во все x86 процессы.
* В x64 ОС также запускается x64 процесс rundll, который загрузит x64\inject.dll, который
* также установит такой хук для x64 процессов.
* 
* x86\inject.dll и x64\inject.dll будут подгружаться операционной системой во все процессы пользователя,
* которые имеют окна. При первом вызове хука по списку подключаемых модулей в shared memory
* определяется, нужно ли подключать какие-нибудь модули, и если да, то загружается xNN\core_as.dll,
* которой передается инфа о подключаем модуле - имя модуля, параметры запуска, определения препроцессора
*/


// Проверяем, не запущен ли уже стартер
bool isPrevInstanceRunning() {
    return !CreateMutex(NULL, TRUE, L"CoreAsStarter") || GetLastError() == ERROR_ALREADY_EXISTS;
}

void getMyFolder() {
    wchar_t path[MAX_PATH], * ptr = path + GetModuleFileName(hMyInst, path, MAX_PATH);
    while (*--ptr != '\\');
    myFolder = ssw{ path, static_cast<unsigned>(ptr + 1 - path) };
}

// Проверка, запущены ли мы на x64 системе.
bool isOs64() {
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
}

/* Сам стартер x86, поэтому при запуске на x64 системе необходимо запустить инжектор
* отдельным 64-битным процессом. Для этого используем штатную утилиту rundll32,
* которая и загрузит нашу inject64.dll. Для взаимодействия с ней создадим канал,
* передав хендлы на них в командной строке
*/
void startInject64() {
    if (!isOs64())
        return;
    // Создаем канал с наследуемыми хендлами, тогда они сразу будут в дочернем процессе, с такими же значениями
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    HANDLE h64Read;
    CreatePipe(&h64Read, &h64Write, &saAttr, 1);
    // Построим командную строку для запуска x64 процесса
    wchar_t winDir[MAX_PATH];
    lstringw<MAX_PATH * 2 + 100> cmdLine(+L"\""_ss &
        ssw { winDir, GetWindowsDirectory(winDir, MAX_PATH) } &
        L"\\Sysnative\\rundll32.exe\" \"" & myFolder & L"lib\\x64\\inject.dll\" installHook " &
        reinterpret_cast<size_t>(h64Read) & L" " & reinterpret_cast<size_t>(h64Write));
    // Запускаем процесс
    STARTUPINFO si;
    si.cb = sizeof(si);
    GetStartupInfo(&si);
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, 0, 0, &si, &pi)) {
        CloseHandle(h64Write);
        h64Write = nullptr;
        MessageBox(0, L"Не удалось запустить x64 инжектор", AppName, MB_OK);
    } else {
        // Закрываем хендлы процесса и потока
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    // Закрываем читающий конец канала.
    // В inject64.dll будет сразу закрыт пишущий конец канала, поэтому
    // при прерывании процесса стартера канал сразу будет закрыт, что
    // приведет к выходу и в дочернем процессе.
    CloseHandle(h64Read);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR) TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR) TRUE;
        }
        break;
    }
    return (INT_PTR) FALSE;
}

void showContextMenu(POINT pt) {
    HMENU menu = LoadMenu(hMyInst, MAKEINTRESOURCE(IDC_STARTER));
    HMENU hPopup = GetSubMenu(menu, 0);
    TrackPopupMenuEx(hPopup, 0, pt.x, pt.y, hMainWnd, NULL);
}

bool showNotify(const wchar_t* msg) {
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON | NIIF_NOSOUND;
    wcscpy(nid.szInfoTitle, AppName);
    wcscpy(nid.szInfo, msg);
    LoadIconMetric(hMyInst, MAKEINTRESOURCE(IDI_STARTER), LIM_LARGE, &nid.hBalloonIcon);
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void processMsgFromOther(const wchar_t* msg) {
    //showNotify(msg);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_NCCREATE:
        hMainWnd = hWnd;
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hMyInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam)) {
        /*case NIN_SELECT:
            ShowWindow(hMainWnd, SW_SHOW);
            break;*/
        case WM_CONTEXTMENU: {
            POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
            showContextMenu(pt);
            break;
        }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SETTEXT:
        processMsgFromOther((const wchar_t*) lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass() {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hMyInst;
    wcex.hIcon = LoadIcon(hMyInst, MAKEINTRESOURCE(IDI_STARTER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = wcex.hIcon;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance() {
    HWND hWnd = CreateWindowW(szWindowClass, AppName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hMyInst, nullptr);
    if (!hWnd) {
        MessageBox(0, L"Не удалось создать основное окно!", AppName, 0);
        return FALSE;
    }
    return TRUE;
}

bool createTrayIcon() {
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hMainWnd;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    LoadIconMetric(hMyInst, MAKEINTRESOURCE(IDI_STARTER), LIM_SMALL, &nid.hIcon);
    wcscpy(nid.szTip, AppName);
    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        MessageBox(0, L"Не удалось создать иконку в трее", AppName, 0);
        return false;
    }
    nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIcon(NIM_SETVERSION, &nid);
    return true;
}

BOOL DeleteNotificationIcon() {
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(nid);
    nid.hWnd = hMainWnd;
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    if (isPrevInstanceRunning()) {
        MessageBox(0, L"Экземпляр стартера уже запущен", AppName, MB_OK);
        return 1;
    }
    hMyInst = hInstance;
    getMyFolder();
    MyRegisterClass();
    if (!InitInstance())
        return 1;
    if (!createTrayIcon())
        return 1;

    coreModule = core_as_getModule(L"starter");
    if (!coreModule || !coreModule->run(nullptr, nullptr)) {
        DeleteNotificationIcon();
        MessageBox(0, L"Не удалось запустить модуль Starter", AppName, MB_OK);
        return 1;
    }

    if (!prepareModulesList()) {
        DeleteNotificationIcon();
        MessageBox(0, L"Не удалось создать список загрузки модулей. Нечего загружать.", AppName, MB_OK);
        return 1;
    }
    inject_hook();
    startInject64();

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DeleteNotificationIcon();
    return (int) msg.wParam;
}
