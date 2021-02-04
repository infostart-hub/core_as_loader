/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Прекомпиленные заголовки
*/

#include "pch.h"
HINSTANCE hMyInst;
stringw myFolder;
CoreAsModule* coreModule;

// we need commctrl v6 for LoadIconMetric()
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

void* operator new(size_t size) {
    return core_as_malloc(size);
}

void operator delete(void* ptr) {
    core_as_free(ptr);
}

