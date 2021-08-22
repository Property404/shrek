#pragma once
#include <stdbool.h>

// Stop forever
[[noreturn]] void halt();

// Kernel panic!
void panic(const char* fmt, ...);
