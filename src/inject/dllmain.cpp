/*
* (c) проект "Core.As", Александр Орефков orefkov@gmail.com
* Точка входа в библиотеку инжектора
*/

#include "pch.h"
#include "../include/inject.h"
#include "../include/core_as/core_as.h"

HINSTANCE hMyInst;
HHOOK hHook = 0;
atomic<unsigned> loadTested = 0;

void getMyFolder(wchar_t* folder) {
    folder += GetModuleFileName(hMyInst, folder, MAX_PATH);
    while (folder[-1] != '\\')
        folder--;
    *folder++ = '.';
    *folder++ = '.';
    *folder++ = '\\';
    *folder++ = '.';
    *folder++ = '.';
    *folder++ = '\\';
    *folder = 0;
}

// Функция загружает в процесс модули Core.As в случае если это задано в настройках
void testNeedLoad() {
    // При запуске стартера он готовит данные о загружаемых модулях и размещает их в shared memory
    // Сейчас мы их получим
    HANDLE hMemory = OpenFileMapping(FILE_MAP_READ, FALSE, LOAD_LIST_MEM_NAME);
    if (!hMemory)
        return;
    const char* data = (const char*) MapViewOfFile(hMemory, FILE_MAP_READ, 0, 0, LOAD_LIST_INIT_SIZE);
    if (!data) {
        CloseHandle(hMemory);
        return;
    }
    // Первым делом идет общий размер блока памяти
    unsigned allSize = *(unsigned*) data;
    if (allSize > LOAD_LIST_INIT_SIZE) {
        UnmapViewOfFile(data);
        data = (const char*) MapViewOfFile(hMemory, FILE_MAP_READ, 0, 0, allSize);
        if (!data) {
            CloseHandle(hMemory);
            return;
        }
    }
    // Затем хэндл окна стартера
    HWND hStarterWnd = reinterpret_cast<HWND>((size_t)*(unsigned*) (data + 4));
    // При первом вызове надо передать в стартер описатель потока, чтобы при выключении
    // он послал в него пустое сообщение, чтобы наша dll выгрузилась.
    {
        wchar_t buf[101];
        swprintf(buf, 100, L"0 %d", GetCurrentThreadId());
        SendMessage(hStarterWnd, WM_SETTEXT, 0, (LPARAM) buf);
    }


    // После приостановки и возобновлении подгрузки функция может вызываться для процесса,
    // в который уже загружены модули. Если это так, то ничего не делаем.
    if (GetModuleHandle(L"core_as.dll"))
        return;

    // Затем идет количество проверяемых модулей
    unsigned modulesCount = *(unsigned*) (data + 8);
    // Получим путь к нашей папке
    wchar_t myFolder[MAX_PATH], buffer[MAX_PATH];
    getMyFolder(myFolder);
    // Получим имя процесса
    wchar_t *fileName = buffer + GetModuleFileName(NULL, buffer, MAX_PATH);
    while (fileName[-1] != '\\')
        fileName--;

    // Загружаемая библиотека Core.As и функции в ней
    typedef CoreAsModule* (*CORE_AS_INIT)(const wchar_t*, const wchar_t*);
    HINSTANCE hCoreAs = nullptr;
    CORE_AS_INIT coreInit = nullptr;

    for (unsigned i = 0; i < modulesCount; i++) {
        LoadModulesData* modul = (LoadModulesData*) (data + 12 + i * sizeof(LoadModulesData));
        bool bSkipModule = true;
        const wchar_t* ptr = (const wchar_t*) (data + modul->processes);
        // Проверяем имя процесса по списку
        for (unsigned p = 0; p < modul->procCount; p++) {
            if (*ptr == '*') { // Если имя начинается с *, то подходит любой процесс
                bSkipModule = false;
                break;
            } else {
                int negate = 0;
                if (*ptr == ':') { // Если имя начинается с :, то проверять на несовпадение
                    negate = 1;
                    ptr++;
                }
                int eql = lstrcmpi(ptr, fileName) == 0 ? 1 : 0;
                if (eql ^ negate) {
                    bSkipModule = false;
                    break;
                }
            }
            while (*ptr)
                ptr++;
            ptr++;
        }
        if (bSkipModule)
            continue;
        const wchar_t* moduleName = (const wchar_t*) (data + modul->moduleName);
        if (modul->testLib) {
            bSkipModule = true;
            const wchar_t* testLib = (const wchar_t*) (data + modul->testLib);
            const char* testFunc = (const char*) (data + modul->testFunc);
            wchar_t dllPath[MAX_PATH];
        #ifndef _WIN64
            wsprintf(dllPath, L"%smodules\\%s\\lib\\x86\\%s", myFolder, moduleName, testLib);
        #else
            wsprintf(dllPath, L"%smodules\\%s\\lib\\x64\\%s", myFolder, moduleName, testLib);
        #endif // !_WIN64
            HINSTANCE hLib = LoadLibrary(dllPath);
            if (hLib) {
                bool(*func)() = (bool(*)())GetProcAddress(hLib, testFunc);
                if (func && func())
                    bSkipModule = false;
                FreeLibrary(hLib);
            }
            if (bSkipModule)
                continue;
        }
        // Надо загружать модуль
        if (!hCoreAs) {
            wchar_t dllPath[MAX_PATH];
        #ifndef _WIN64
            wsprintf(dllPath, L"%slib\\x86\\core_as.dll", myFolder);
        #else
            wsprintf(dllPath, L"%slib\\x64\\core_as.dll", myFolder);
        #endif
            hCoreAs = LoadLibrary(dllPath);
            if (!hCoreAs)
                goto err;
            coreInit = (CORE_AS_INIT) GetProcAddress(hCoreAs, "core_as_getModule");
            if (!coreInit)
                goto err;
        }
        CoreAsModule* coreModule = coreInit(moduleName, nullptr);
        if (coreModule) {
            unsigned hProcess = (unsigned) (size_t) OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
            coreModule->setParam(L"StarterWnd", reinterpret_cast<uint64_t>(hStarterWnd));
            coreModule->setParam(L"ProcessHandle", hProcess);
            const wchar_t* args = modul->args ? (const wchar_t*) (data + modul->args) : nullptr;
            const wchar_t* defs = modul->defines? (const wchar_t*) (data + modul->defines) : nullptr;
            if (coreModule->run(args, defs)) {
                wchar_t buf[301];
                swprintf(buf, 300, L"1 %d \"%s\" %s", hProcess, fileName, moduleName);
                SendMessage(hStarterWnd, WM_SETTEXT, 0, (LPARAM)buf);
            }
        }
    }
err:
    UnmapViewOfFile(data);
    CloseHandle(hMemory);
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (0 == loadTested.fetch_add(1, memory_order_relaxed)) {
        // Проверяем, не нужно ли загрузить Core.As
        testNeedLoad();
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// x86 стартер вызывает эти функции напрямую, для внедрения в x64 процессы
// запускается rundll32 с вызовом функции installHook, которая ставит хук
// и ожидает команды от стартера для запуска/останова хука
INJECT_FUNC
void inject_hook() {
    if (!hHook)
        hHook = SetWindowsHookEx(WH_CBT, CBTProc, hMyInst, 0);
}

INJECT_FUNC
void inject_unhook() {
    if (hHook) {
        UnhookWindowsHookEx(hHook);
        hHook = 0;
    }
}


#ifdef _WIN64
// Функция инициализации установки хука. Имеет такую сигнатуру для возможности
// вызова через rundll32, когда необходимо установить хук для другой архитектуры
extern "C" __declspec(dllexport)
void CALLBACK installHook(HWND, HINSTANCE, LPSTR pCmdLine, int) {
    inject_hook();
    char* ptr;
    HANDLE commandPipe = (HANDLE)(unsigned __int64)strtol(pCmdLine, &ptr, 0);
    HANDLE writePipe = (HANDLE) (unsigned __int64) strtol(ptr, &ptr, 0);
    CloseHandle(writePipe);
    for (;;) {
        char cmd;
        DWORD readed;
        if (!ReadFile(commandPipe, &cmd, 1, &readed, 0) || !readed)
            return;
        switch (cmd) {
        case icExit:
            return;
        case icHook:
            inject_hook();
            break;
        case icUnhook:
            inject_unhook();
            break;
        }
    }
}
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
    if (DLL_PROCESS_ATTACH == ul_reason_for_call) {
        hMyInst = hModule;
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}
