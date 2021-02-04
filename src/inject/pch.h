/*
* (c) проект "Core.As", Александр Орефков orefkov@gmail.com
* Прекомпилируемые заголовки
*/

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#ifdef _WIN64
#define MY_NAME L"x64 "
#else
#define MY_NAME L"x86 "
#endif

#endif //PCH_H
