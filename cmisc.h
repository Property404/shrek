#pragma once
#include <stdint.h>
#include <stddef.h>
// Standard C library function replacements
//
// Prototypes may be slightly different
// and do not necessarily conform to C or POSIX
// standards

void memset(void* dest, uint8_t byte, size_t length);
void memcpy(void*restrict dest, const void*restrict src, size_t length);

int strncmp(const char* s1, const char* s2, size_t length);

