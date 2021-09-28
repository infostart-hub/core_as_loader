/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Отдельно выполняемый gui запускальщик модулей Core.As
* 
*/
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// std
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <locale>
#include <clocale>
#include <cwchar>

#include "core_as/core_as.h"
void* operator new(size_t size) {
    return core_as_malloc(size);
}

void operator delete(void* ptr) {
    core_as_free(ptr);
}

int showUsage() {
    MessageBox(0, LR"aaa(Core.As Runner. Программа для запуска модулей Core.As.
Используйте одну из команд:
run   <ИмяМодуля> [параметры]    - Запуск модуля
check <ИмяМодуля> [параметры]    - Проверка модуля

Параметры передаются запускаемому модулю в виде:
Параметр /f - задаёт папку с модулем
Каждый аргумент '/d строка' или '-d строка' передается как определение
для препроцессора модуля, например:
/d IDE=1 /d USECOOLFEATURE -D "POWER = 100"

Все параметры после /c или -c - передаются модулю как аргументы его запуска.

Параметры с пробелами заключайте в кавычки. Кавычки внутри параметров удваивайте.
Например:
coreas_runner_w run sendfile /c -subj "Мой корабль плывёт как ""Титаник"""

Также модуль может сам получать дополнительные настройки из переменных окружения
вида COREAS_ИмяНастройки=значение настройки
и аргументов командной строки вида -coreas-ИмяНастройки "значение настройки"
Имена настроек регистронезависимые.
)aaa", L"Core.As", MB_OK);

    return 1;
}

// Выяснилось, что процедура разбора командных аргументов штатного консольного cruntime при наличии слэша перед
// закрывающей кавычкой "проглатывает" её и прихватывает соседние аргументы. Это, во-первых, отличается
// от поведения моих разборщиков командной строки в других частях программы, во-вторых, делает неудобным
// передачу директорий в командной строке. Поэтому будем разбирать командную строку сами.
void processArgs(lstringw<300>& defines, lstringw<300>& commands, lstringw<MAX_PATH>& folder, const std::vector<ssw>& argv) {
    uint argc = uint(argv.size());
    std::vector<ssw> vdef, vcmd;
    if (argc > 4) {
        vdef.reserve((argc - 3) / 2);
        vcmd.reserve(argc - 4);
    }
    for (uint i = 3; i < argc - 1; i++) {
        if (argv[i].isEqualia(L"/d") || argv[i].isEqualia(L"-d"))
            vdef.emplace_back(argv[++i]);
        else if (argv[i].isEqualia(L"/f") || argv[i].isEqualia(L"-f"))
            folder = argv[++i];
        else if (argv[i].isEqualia(L"/c") || argv[i].isEqualia(L"-c")) {
            while (++i < argc)
                vcmd.emplace_back(argv[i]);
        }
    }
    defines.s_join(vdef, L"\v").s_replace(L"\"\"", L"\"");
    commands.s_join(vcmd, L"\v").s_replace(L"\"\"", L"\"");
}

int run(const std::vector<ssw>& argv, HINSTANCE hInst, int nCmdShow) {
    lstringw<300> defines, commands;
    lstringw<MAX_PATH> folder;
    processArgs(defines, commands, folder, argv);
    lstringw<MAX_PATH> modName(argv[2]);
    CoreAsModule* pModule = core_as_getModule(modName, folder);
    if (!pModule)
        return 2;
    commands.s_append(+L" hInst "_ss & size_t(hInst) & L" nCmdShow " & nCmdShow);
    return pModule->run(commands, defines, GetStdHandle(STD_OUTPUT_HANDLE)) ? 0 : 1;
}

int check(const std::vector<ssw>& argv) {
    lstringw<300> defines, commands;
    lstringw<MAX_PATH> folder;
    processArgs(defines, commands, folder, argv);
    lstringw<MAX_PATH> modName(argv[2]);
    CoreAsModule* pModule = core_as_getModule(modName, folder);
    if (!pModule)
        return 2;
    return pModule->check(defines, GetStdHandle(STD_OUTPUT_HANDLE)) ? 0 : 1;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nShowCmd) {
    setlocale(LC_ALL, "ru_RU.utf8");
    std::vector<ssw> argv = core_as_parseArguments(e_s(lpCmdLine));
    if (argv.size() > 2) {
        if (argv[1] == L"run") {
            return run(argv, hInstance, nShowCmd);
        } else if (argv[1]== L"check") {
            return check(argv);
        }
    }
    return showUsage();
}
