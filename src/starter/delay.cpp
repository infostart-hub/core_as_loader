/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Реализация отложенной загрузки разных.dll
* Так как у стартера линковка с core_as.dll и inject.dll, которые не размещены в
* каталоге программы, мы их отмечаем для линкера как "отложенные", и подгружаем их
* в случае необходимости сами.
*/
#include "pch.h"
#include <delayimp.h>

FARPROC WINAPI delayLoadHook(unsigned dliNotify, PDelayLoadInfo pdli) {
    if (dliNotePreLoadLibrary == dliNotify) {
        wchar_t szLibName[MAX_PATH];
        wsprintf(szLibName, L"%slib\\x86\\%S", myFolder.c_str(), pdli->szDll);
        return (FARPROC) LoadLibrary(szLibName);
    }
    return 0;
}

extern "C" const PfnDliHook __pfnDliNotifyHook2 = delayLoadHook;
