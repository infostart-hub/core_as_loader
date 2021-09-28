/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Функции для подготовки списка загружаемых модулей
*/
#include "pch.h"

/*
* При старте программа сканирует каталог load и обрабатывает все txt файлы из него
* В файлах описывается, какие модули к каким процессам подключать
* Формат файла - ini файл:
* [имя секции произвольное]
* proc= список процессов через запятую, названия исполняемых файлов, которые нужно проверять на подключение
*       если * - то все процессы, если начинается с ! - не грузить
* test= имя dll,имя функции, При наличии загружается в нужный процесс и решает, надо ли подключать модуль.
* modul= имя модуля
* args= параметры запуска модуля, передаются в main
* defs=список определений препроцессора через запятую name1:val1,name2:val2
*/
HANDLE hListMap;
char* pListMap;

struct LoadModuleInfo {
    LoadModuleInfo() = default;
    LoadModuleInfo(stringw&& mn, std::vector<stringw>&& pr) : moduleName(mn), processes(pr) {}
    stringw moduleName;
    std::vector<stringw> processes;
    stringw testLib;
    stringw testFunc;
    stringw args;
    stringw defs;
};

stringw parseArguments(ssw args) {
    return stringw{ eew & e_repl(lstringw<500>::join(core_as_parseArguments(args), L"\v").to_str(), L"\"\"", L"\"") };
}

void processFile(const wchar_t* filePath, std::vector<LoadModuleInfo>& modules) {
    lstringw<0x200> sectionNames{ [filePath](wchar_t* p, unsigned s) {
        return grow2(GetPrivateProfileString(NULL, NULL, NULL, p, s + 1, filePath), s - 1);
    } };
    lstringsw<0x200> buffer;
    
    for (const wchar_t* section = sectionNames; *section; section += std::char_traits<u16symbol>::length(section) + 1) {
        buffer << [&](wchar_t* p, uint s) {
            return grow2(GetPrivateProfileString(section, L"modul", NULL, p, s + 1, filePath), s);
        };
        if (!buffer)
            continue;
        stringw modulName{ std::move(buffer) };

        auto procList = (buffer << [&](wchar_t* p, uint s) {
            return grow2(GetPrivateProfileString(section, L"proc", NULL, p, s + 1, filePath), s);
            }).splitf<std::vector<stringw>>(L",", trim_w{});
        
        if (procList.empty())
            continue;

        LoadModuleInfo info{ std::move(modulName), std::move(procList) };

        auto lib = (buffer << [&](wchar_t* p, uint s) {
            return grow2(GetPrivateProfileString(section, L"test", NULL, p, s + 1, filePath), s);
            }).splitf<std::vector<stringw>>(L",", trim_w{});
        if (lib.size() == 2) {
            info.testLib = std::move(lib[0]);
            info.testFunc = std::move(lib[1]);
        }
        
        info.args = parseArguments(buffer << [&](wchar_t* p, uint s) {
            return grow2(GetPrivateProfileString(section, L"args", NULL, p, s, filePath), s);
            });
        info.defs = parseArguments(buffer << [&](wchar_t* p, uint s) {
            return grow2(GetPrivateProfileString(section, L"defs", NULL, p, s, filePath), s);
            });
        modules.emplace_back(std::move(info));
    }
}

void storeA(char*& ptr, const stringw& s) {
    const wchar_t* text = s.c_str();
    unsigned l = s.length() + 1;
    while (l--)
        *ptr++ = (char)*text++;
    if (reinterpret_cast<size_t>(ptr) & 1)
        *ptr++ = 0;
}

bool layoutListToMemory(const std::vector<LoadModuleInfo>& modules) {
    DWORD size = 8 + sizeof(HWND) + sizeof(LoadModulesData) * (DWORD) modules.size();

    for (const auto& m: modules) {
        size += m.moduleName.length() * 2 + 2;
        for (const auto& p: m.processes)
            size += p.length() * 2 + 2;
        if (m.testLib.length())
            size += m.testLib.length() * 2 + 2 + ((m.testFunc.length() + 2) & ~1);
        if (m.args.length())
            size += m.args.length() * 2 + 2;
        if (m.defs.length())
            size += m.defs.length() * 2 + 2;
    }
    hListMap = CreateFileMapping(NULL, 0, PAGE_READWRITE, 0, size < LOAD_LIST_INIT_SIZE ? LOAD_LIST_INIT_SIZE : size, LOAD_LIST_MEM_NAME);
    if (!hListMap)
        return false;
    pListMap = (char*) MapViewOfFile(hListMap, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (!pListMap) {
        CloseHandle(hListMap);
        hListMap = NULL;
        return false;
    }
    *(unsigned*) pListMap = size;
    *(unsigned*) (pListMap + 4) = (unsigned) hMainWnd;
    *(unsigned*) (pListMap + 8) = (unsigned)modules.size();
    LoadModulesData* modul = (LoadModulesData*) (pListMap + 12);
    char* pStrings = pListMap + 12 + sizeof(LoadModulesData) * modules.size();
    for (const auto& m: modules) {
        modul->moduleName = static_cast<unsigned>(pStrings - pListMap);
        m.moduleName.store(pStrings);
        modul->procCount = static_cast<unsigned>(m.processes.size());
        modul->processes = static_cast<unsigned>(pStrings - pListMap);
        for (const auto& p: m.processes)
            p.store(pStrings);
        if (m.testLib.length()) {
            modul->testLib = static_cast<unsigned>(pStrings - pListMap);
            m.testLib.store(pStrings);
            modul->testFunc = static_cast<unsigned>(pStrings - pListMap);
            storeA(pStrings, m.testFunc);
        } else
            modul->testLib = modul->testFunc = 0;
        if (m.args.length()) {
            modul->args = static_cast<unsigned>(pStrings - pListMap);
            m.args.store(pStrings);
        }
        if (m.defs.length()) {
            modul->defines = static_cast<unsigned>(pStrings - pListMap);
            m.defs.store(pStrings);
        }
        modul++;
    }
    return true;
}

bool prepareModulesList() {
    lstringw<MAX_PATH> path = +myFolder & L"load\\*.txt" ;
    WIN32_FIND_DATA fnd;
    HANDLE hFnd = FindFirstFile(path, &fnd);
    if (INVALID_HANDLE_VALUE != hFnd) {
        std::vector<LoadModuleInfo> modules;
        do {
            processFile(path.assign(+myFolder & L"load\\" & e_s(fnd.cFileName)), modules);
        } while (FindNextFile(hFnd, &fnd));
        FindClose(hFnd);
        if (!modules.empty())
            return layoutListToMemory(modules);
    } else
        coreModule->log<LogLevel::Critical>("Not open find in prepareModulesList. Error 0x%08X", GetLastError());
    return false;
}
