/*
* (c) проект "Core.As", Александр Орефков orefkov@gmail.com
* Описание экспортируемого из inject.dll API
*/

#pragma once
#ifdef INJECT_EXPORTS
#define INJECT_FUNC __declspec(dllexport)
#else
#define INJECT_FUNC __declspec(dllimport)
#endif

INJECT_FUNC void inject_hook();
INJECT_FUNC void inject_unhook();

enum InjectCommands {
    icExit,
    icHook,
    icUnhook,
};

/*
* Для проверки, нужно ли загружать модуль в этот процесс, используется список в общей памяти,
* который готовит один раз стартер, а затем во всех процессах по нему идет проверка.
* Структура блока памяти
*   4 байта - размер блока (минимально выделяется LOAD_LIST_INIT_SIZE, если больше - переоткрыть блок)
*   4 байта - количество блоков описания модулей
*   Массив блоков описаний модулей. Каждое описание - фиксированной длины. В описании хранятся смещения от
*   начала всего общего блока до строк
*   Массив строк
* 
* Формат описания модуля
*   смещение до имени модуля
*   количество процессов
*   смещение до имен процессов
*   смещение до имени тест-либы/тестфункции
*   смещение до аргументов
*   смещение до дефайнов
*/

struct LoadModulesData {
    unsigned moduleName;
    unsigned procCount;
    unsigned processes;
    unsigned testLib;
    unsigned testFunc;
    unsigned args;
    unsigned defines;
};

#define LOAD_LIST_MEM_NAME L"CoreAsStarterList"
#define LOAD_LIST_INIT_SIZE 0x1000

