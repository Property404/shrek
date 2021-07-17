#pragma once
#include <stdbool.h>

struct SerialDriver {
    bool(*testchar)(void);
    void(*putchar)(char);
    int(*getchar)(void);
};

extern SerialDriver serial_driver;
