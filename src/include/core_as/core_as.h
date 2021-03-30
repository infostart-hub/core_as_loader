/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Описание основного экспортируемого из core_as.dll API
*/
#include "sstring.h"
#pragma once

class CoreAsModule {
protected:
    LogLevel currentLogLevel{ LogLevel::Normal };
    CoreAsModule() = default;
    virtual void doLog(LogLevel level, ssa msg) = 0;
    virtual void doLog(LogLevel level, ssw msg) = 0;
public:
    virtual ~CoreAsModule() {};
    virtual const stringw& name() const = 0;
    // Вызывается когда нужно только проверить синтаксис модуля.
    // Передаётся хендл потока, в который выводить сообщения об ошибках
    virtual bool check(const wchar_t* defines, HANDLE hOut) = 0;
    // Вызывается когда нужно сделать h-файл с дампом API модуля
    virtual bool dumpApi(const wchar_t* defines, HANDLE hOut) = 0;
    // Вызывается когда нужно запустить модуль на выполнение.
    // Может передавать хэндл потока, в который выводить сообщения об ошибках и Print.
    // Если не указан, то они выводятся в лог-консоль
    virtual bool run(const wchar_t* args, const wchar_t* defines, HANDLE hOut = nullptr) = 0;
    // Позволяет перед запуском установить для модуля дополнительные параметры, которые
    // тот сможет получить в рантайме
    virtual void setParam(const wchar_t* name, const wchar_t* val) = 0;
    virtual void setParam(const wchar_t* name, uint64_t val) = 0;

    LogLevel currentLL() const { return currentLogLevel; }
    // Вспомогательные функции для вывода в лог
    template<LogLevel LL, typename T, typename...A>
    void log(const u8symbol* format, T&& arg1, A&&...args) {
        if (currentLogLevel >= LL)
            doLog(LL, lstringa<300>().s_format(format, forward<T>(arg1), forward<A>(args)...));
    }
    
    template<LogLevel LL, typename T, typename...A>
    void log(const u16symbol* format, T&& arg1, A&&...args) {
        if (currentLogLevel >= LL)
            doLog(LL, lstringw<300>().s_format(format, forward<T>(arg1), forward<A>(args)...));
    }

    template<LogLevel LL>
    void log(ssa msg) {
        if (currentLogLevel >= LL)
            doLog(LL, msg);
    }

    template<LogLevel LL>
    void log(ssw msg) {
        if (currentLogLevel >= LL)
            doLog(LL, msg);
    }

    template<LogLevel LL, typename A, enable_if_t<is_same_v<typename A::symb_type, char>, int> = 0>
    void log(const strexpr<A>& expr) {
        if (currentLogLevel >= LL)
            doLog(LL, lstringa<300>(expr));
    }
    template<LogLevel LL, typename A, enable_if_t<is_same_v<typename A::symb_type, u16symbol>, int> = 0>
    void log(const strexpr<A>& expr) {
        if (currentLogLevel >= LL)
            doLog(LL, lstringw<300>(expr));
    }
};

/*
* Если в модуле загружаются свои длл, то в них ищется функция moduleInit. Должна иметь сигнатуру
* bool moduleInit(LibInitInterface* initIface, CoreAsModule* module, bool onlyCheck)
* Через этот интерфейс длл может зарегистрировать свои объекты в AngelScript API, а также
* добавить дополнительные определения препроцессора.
* После регистрации внешнего API модуля при наличии функции void afterRegisterApi(), она также
* вызывается, чтобы модуль мог дополнительно зарегистрировать объекты, зависящие от его внешнего API
*/
struct LibInitInterface {
    virtual class asIScriptEngine* getAsEngine() = 0;
    virtual void addDefine(const char* define) = 0;
};

extern "C" COREAS_API CoreAsModule* core_as_getModule(const wchar_t* moduleName, const wchar_t* moduleFolder = nullptr);
COREAS_API vector<ssw> core_as_parseArguments(ssw args);
