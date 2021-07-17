#pragma once

// Align down if unaligned
#define ALIGN_DOWN(value, size) ((value) & ~(size-1))

// Align up if unaligned
#define ALIGN_UP(value, size) (((value)%(size))?(ALIGN_DOWN((value+size), (size))):(value))
