#pragma once
#include <stdbool.h>

typedef struct {
    bool(*testchar)(void);
    void(*putchar)(char);
    int(*getchar)(void);
} SerialDriver;

extern SerialDriver serial_driver;

void putchar(char c);
bool testchar(void);
int getchar(void);
