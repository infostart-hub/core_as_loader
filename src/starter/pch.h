/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Прекомпиленные заголовки
*/
#pragma once

#ifdef _WIN64
#error Стартер собирать только x86
#endif
#define _CRT_SECURE_NO_WARNINGS
// add headers that you want to pre-compile here
#include "framework.h"
#include "../include/inject.h"
#include "../include/core_as/core_as.h"


extern HINSTANCE hMyInst;
extern stringw myFolder;
const wchar_t AppName[] = L"Core.As Starter";
extern CoreAsModule* coreModule;
extern HWND hMainWnd;
