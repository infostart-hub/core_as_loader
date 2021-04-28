/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Вход в скриптовый модуль
*/

 // Данные строки нужны только для среды разработки и вырезаются препроцессором
#pragma once
#include "../../all.h"

StarterInterProcess&& stipc;

int main(array<string>&& args) {
    &&stipc = StarterInterProcess("Starter", function(const string& data) {
    });
    stipc.showNotify("Core.As cтартер", "Запущен стартер Core.As");
/*	Print(
"""Запущен модуль стартера Core.As
""");
*/
	return 0;
}
