/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Вход в приложение
*/

#include "pch.h"
#include "starter.h"
#include "modules_list.h"
#include "VersionInfo.h"
#include "debugdump.h"

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
HWND hMainWnd;
// Сюда будем писать команды для x64 инжектора - снять хук/установить хук
HANDLE h64Write = nullptr;
const wchar_t szWindowClass[] = L"Core.As.Starter";
bool isInjected = false;

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
    wchar_t path[MAX_PATH], *ptr = path + GetModuleFileName(hMyInst, path, MAX_PATH);
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
    lstringw<MAX_PATH * 2 + 100> cmdLine{ L"\"" &
        ssw { winDir, GetWindowsDirectory(winDir, MAX_PATH) } &
        L"\\Sysnative\\rundll32.exe\" \"" & myFolder & L"lib\\x64\\inject.dll\" installHook " &
        reinterpret_cast<size_t>(h64Read) & L" " & reinterpret_cast<size_t>(h64Write) };
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
    case WM_INITDIALOG: {
        uint64_t version = VersionInfo{ hMyInst, FALSE }.prodVersion();
        SetDlgItemText(hDlg, IDC_VERSION, lstringw<100>{eew & uint(version >> 48) & L"." &
            uint((version >> 32) & 0xFFFF) & L"." & uint((version >> 16) & 0xFFFF) & L"." &
            uint(version & 0xFFFF)
        #ifdef _DEBUG
            & L" (debug)"
        #endif
        });
        return (INT_PTR) TRUE;
    }
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
    ModifyMenu(hPopup, ID_TOGGLE_INJECT, MF_BYCOMMAND | MF_STRING, ID_TOGGLE_INJECT, isInjected ? L"Приостановить" : L"Возобновить");
    SetForegroundWindow(hMainWnd);
    TrackPopupMenuEx(hPopup, 0, pt.x, pt.y, hMainWnd, NULL);
    DestroyMenu(menu);
}

bool showNotify(ssw title, ssw msg) {
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(nid);
    nid.hWnd = hMainWnd;
    nid.uFlags = NIF_INFO | NIF_SHOWTIP;
    nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON | NIIF_NOSOUND;
    if (title.isEmpty())
        title = AppName;
    title.copy_to(nid.szInfoTitle, sizeof(nid.szInfoTitle) / sizeof(nid.szInfoTitle[0]));
    msg.copy_to(nid.szInfo, sizeof(nid.szInfo) / sizeof(nid.szInfo[0]));

    LoadIconMetric(hMyInst, MAKEINTRESOURCE(IDI_STARTER), LIM_LARGE, &nid.hBalloonIcon);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    DWORD err = GetLastError();
    return true;
}

enum StarterMessages {
    smInject,        // Посылается при загрузке inject.dll в процесс, передает описатель потока
    smLoadModule,    // Посылается из inject.dll
    smConnect,       // Подключение модуля к стартеру, регистрирует его для посылки обратных сообщений
    smDisconnect,    // Отключение модуля от стартера
    smShowNotify,    // Показать уведомление
    smStopInject,    // Команда приостановить внедрение модулей в запускаемые процессы
    smResumeInject,  // Команда возобновить внедрение модулей в запускаемые процессы
    smBrodcast,      // Разослать сообщение другим запущенным модулям
};

struct ConnectedInstance {
    HWND hWnd;
    stringw name;
};

std::vector<ConnectedInstance> connectedInstances;
std::vector<DWORD> injectThreads;

// Ждем строку вида ThreadID
void processInject(SimpleStrNtW msg) {
    injectThreads.push_back(wcstoul(msg, nullptr, 0));
}

// Ждем строку вида ИмяИнстанса\vHWND окна
void processConnect(SimpleStrNtW msg) {
    auto parts = msg.split<std::vector<ssw>>(L"\v");
    if (parts.size() == 2) {
        HWND hWnd = (HWND)wcstol(parts[1].c_str(), nullptr, 0);
        if (IsWindow(hWnd))
            connectedInstances.emplace_back(ConnectedInstance{ hWnd, parts[0] });
    }
}

// Ждем строку вида HWND окна
void processDisconnect(SimpleStrNtW msg) {
    HWND hWnd = (HWND)wcstol(msg, nullptr, 0);
    const auto fnd = std::find_if(connectedInstances.begin(), connectedInstances.end(), [=](const ConnectedInstance& c) {return c.hWnd == hWnd; });
    if (fnd != connectedInstances.end())
        connectedInstances.erase(fnd);
}

// Ждем строку вида Заголовок\vТекст
void processShowNotify(SimpleStrNtW msg) {
    // Данные для показа уведомления идут в виде Заголовок\vТекст
    auto txt = msg.split<std::vector<ssw>>(L"\v");
    if (txt.size() == 2)
        showNotify(txt[0], txt[1]);
}

void processBroadCast(SimpleStrNtW data) {
    for (auto it = connectedInstances.begin(); it != connectedInstances.end(); ) {
        if (!IsWindow(it->hWnd)) {
            connectedInstances.erase(it);
        } else {
            SendMessage(it->hWnd, WM_SETTEXT, 0, (LPARAM) data.c_str());
            ++it;
        }
    }
}

void processStopInject() {
    if (isInjected) {
        inject_unhook();
        if (h64Write) {
            char cmd = icUnhook;
            DWORD w;
            WriteFile(h64Write, &cmd, 1, &w, 0);
        }
        isInjected = false;
        processBroadCast(lstringw<100>{L"starter\v" & e_num<u16symbol>((size_t)hMainWnd) & L"\vinject=0"});
        // При снятии хука inject.dll выгружается из других процессов не сразу, а когда
        // через очередь сообщений потока с хуком пройдёт хотя бы одно сообщение.
        // И для некоторых процессов это ожидание может сильно затянутся. Поэтому при снятии хука
        // надо в каждый поток с inject.dll послать пустое сообщение.
        for (auto threadId: injectThreads)
            PostThreadMessage(threadId, WM_NULL, 0, 0);
        injectThreads.clear();
    }
}

void processResumeInject() {
    if (!isInjected) {
        inject_hook();
        if (h64Write) {
            char cmd = icHook;
            DWORD w;
            WriteFile(h64Write, &cmd, 1, &w, 0);
        }
        isInjected = true;
        processBroadCast(lstringw<100>{L"starter\v" & e_num<u16symbol>((size_t)hMainWnd) & L"\vinject=1"});
    }
}

void processLoadModule(ssw msg) {
    coreModule->log<LogLevel::Info>(L"Load module " & msg);
}

void processMsgFromOther(const wchar_t* msg) {
    wchar_t* end;
    long cmd = wcstol(msg, &end, 0);
    if (*end == ' ')
        ++end;
    SimpleStrNtW tail = e_s(end);
    switch (cmd) 	{
    case smInject:
        processInject(tail);
        break;
    case smLoadModule:
        processLoadModule(tail);
        break;
    case smConnect:
        processConnect(tail);
        break;
    case smDisconnect:
        processDisconnect(tail);
        break;
    case smShowNotify:
        processShowNotify(tail);
        break;
    case smStopInject:
        processStopInject();
        break;
    case smResumeInject:
        processResumeInject();
        break;
    case smBrodcast:
        processBroadCast(tail);
        break;
    }
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
        case ID_TOGGLE_INJECT:
            if (isInjected)
                processStopInject();
            else
                processResumeInject();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam)) {
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
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
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


/* Так как вылет может произойти еще на этапе инициализации cruntime,
*  то зададим свою точку входа в программу, поставим свой фильтр на исключения
*  и вызовем cruntime
*/
extern "C" void wWinMainCRTStartup();

extern "C" void realEntry() {
    initLastDump();
    wWinMainCRTStartup();
    // Сюда не должны приходить
    ExitProcess(0);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    // cruntime могла заменить наши фильтры, восстановим их
    initLastDump();
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
    isInjected = true;
    inject_hook();
    startInject64();

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DeleteNotificationIcon();
    processStopInject();
    //coreModule->stop();
    return (int) msg.wParam;
}

extern "C" __declspec(dllexport)
bool moduleInit(LibInitInterface* li, CoreAsModule* module, bool forCheck) {
    /*
    asEngine = li->getAsEngine();
    AsRegister::registerAll(AsInitLevel::typeNames, false);
    AsRegister::registerAll(AsInitLevel::enumVals, false);
    AsRegister::registerAll(AsInitLevel::ctors, false);
    AsRegister::registerAll(AsInitLevel::typeMembers, false);
    */
    return true;
}
