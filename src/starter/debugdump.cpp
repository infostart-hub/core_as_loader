/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Сброс минидампа при падениях
*/
#include "pch.h"

#ifdef _DEBUG
#include <Dbghelp.h>

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP) (HANDLE,
    DWORD,
    HANDLE,
    MINIDUMP_TYPE,
    PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION,
    PMINIDUMP_CALLBACK_INFORMATION);

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);

static bool dumped = false;

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExceptionInfo) {
    if (!dumped) {
        dumped = true;
        HMODULE hDll = LoadLibrary(L"dbghelp.dll");
        if (hDll) {
            MINIDUMPWRITEDUMP pfnDump = (MINIDUMPWRITEDUMP)GetProcAddress(hDll, "MiniDumpWriteDump");
            if (pfnDump) {
                wchar_t buf[MAX_PATH], *ptr = buf + GetModuleFileName(nullptr, buf, MAX_PATH);
                lstrcpy(ptr, L".dmp");
                HANDLE hFile = CreateFile(buf,
                    GENERIC_WRITE,
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
                if (hFile != INVALID_HANDLE_VALUE) {
                    MINIDUMP_EXCEPTION_INFORMATION exInfo {
                        .ThreadId = GetCurrentThreadId(),
                        .ExceptionPointers = pExceptionInfo,
                        .ClientPointers = 0
                    };
                    // Write pDumpFile
                    pfnDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MINIDUMP_TYPE(MiniDumpNormal | MiniDumpWithHandleData), &exInfo, NULL, NULL);
                    CloseHandle(hFile);
                }
            }
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void initLastDump() {
    AddVectoredExceptionHandler(1, TopLevelFilter);
    SetUnhandledExceptionFilter(TopLevelFilter);
}

#endif
