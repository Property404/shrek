#pragma once
#include <stdbool.h>
#include <stdarg.h>

extern "C" int putchar(int c);
bool testchar(void);
extern "C" int getchar(void);
extern "C" int puts(const char* buffer);
extern "C" int printf(const char* fmt, ...);
extern "C" int vprintf(const char* fmt, va_list va_args);
