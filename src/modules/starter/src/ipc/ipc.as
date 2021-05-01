/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Реализация общего для других модулей кода по взаимодействию со стартером
*/

 // Данные строки нужны только для среды разработки и вырезаются препроцессором
#pragma once
#include "../../../all.h"

/*
* При запуске стартер создает окно с указанным именем класса.
* Взаимодействие между стартером и другими модулями осуществляется посылкой
* сообщения WM_SETTEXT в это окно.
*/
const string StarterWindowClassName = "Core.As.Starter";

// Коды видов сообщений
enum StarterMessages {
    smInject,        // Посылается при загрузке inject.dll в процесс, передает описатель потока
    smLoadModule,    // Посылается из inject.dll
    smConnect,       // Подключение модуля к стартеру, регистрирует его для посылки обратных сообщений
    smDisconnect,    // Отключение модуля от стартера
    smShowNotify,    // Показать уведомление
    smStopInject,    // Команда приостановить внедрение модулей в запускаемые процессы
    smResumeInject,  // Команда возобновить внедрение модулей в запускаемые процессы
    smBrodcast,      // Разослать сообщение другим запущенным модулям
};

funcdef void StarterNotify(const string& data);

class StarterInterProcess {
    StarterInterProcess(const string& instanceName, StarterNotify&& callback) {
        &&callback_ = callback;
        connectToStarter(instanceName);
    }
    ~StarterInterProcess() {
        if (isConnected())
            sendMessageToStarter(smDisconnect, "" + hMyWnd);
    }

    bool isConnected() const {
        return IsWindow(hStarterWnd) != 0;
    }

    void showNotify(const string& title, const string& message) {
        sendMessageToStarter(smShowNotify, title + "\xB" + message);
    }

    void setInjectStatus(bool inject) {
        sendMessageToStarter(inject ? smResumeInject : smStopInject, "");
    }

    void sendBroadcast(const string& source, const string& data) {
        sendMessageToStarter(smBrodcast, source + "\xB" + hMyWnd + "\xB" + data);
    }

    private void sendMessageToStarter(StarterMessages type, const string& data) {
        if (isConnected()) {
            string msg = "" + type + " " + data;
            SendMessage(hStarterWnd, WM_SETTEXT, 0, msg.cstr);
        }
    }

    private void connectToStarter(const string& instanceName) {
        hStarterWnd = FindWindow(StarterWindowClassName.cstr, 0);
        if (hStarterWnd == 0) {
            Print("No find starter wnd");
            return;
        }
        hMyWnd = CreateWindowEx(0, "EDIT".cstr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        &&myWnd = attachWndToFunction(hMyWnd, WndFunc(this.wndFunc), array<uint> = { WM_SETTEXT });
        sendMessageToStarter(smConnect, instanceName + "\xB" + hMyWnd);
    }

    private LRESULT wndFunc(uint msg, WPARAM w, LPARAM l) {
        switch (msg) {
        case WM_SETTEXT:
            callback_(stringFromAddress(l));
            break;
        }
        return myWnd.doDefault();
    }

    StarterNotify&& callback_;
    HWND hStarterWnd = 0;
    HWND hMyWnd = 0;
    ASWnd&& myWnd;
};
