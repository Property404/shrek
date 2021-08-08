#pragma once
#include <stdbool.h>
#include <stdarg.h>

void puts(const char* buffer);
void putchar(char c);
bool testchar(void);
int getchar(void);
void printf(const char* fmt, ...);
void vprintf(const char* fmt, va_list va_args);
