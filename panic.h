#pragma once
#include <stdbool.h>

// Stop forever
void halt();

// Kernel panic!
void panic(const char* fmt);
