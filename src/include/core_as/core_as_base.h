/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Описание базового экспортируемого из core_as.dll API
*/
#pragma once

#pragma once
#ifdef COREAS_EXPORTS
#define COREAS_API __declspec(dllexport)
#else
#define COREAS_API __declspec(dllimport)
#endif


enum class LogLevel {
    Critical,
    Warning,
    Normal,
    Info,
    Diagnostic
};

COREAS_API void* core_as_malloc(size_t count);
COREAS_API void* core_as_realloc(void* ptr, size_t count);
COREAS_API void core_as_free(void* ptr);
