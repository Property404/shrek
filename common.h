#pragma once
#include <cstdint>
#include <typeinfo>
#include "panic.h"

// Align down if unaligned
#define ALIGN_DOWN(value, size) ((value) & ~(size-1))

// Align up if unaligned
#define ALIGN_UP(value, size) (((value)%(size))?(ALIGN_DOWN((value+size), (size))):(value))

// Check alignment
#define ALIGNED_TO(value, size) (!((value) & (size-1)))

// Convert big endian data to native endianness (little endian)
template <typename T>
T big_endian_to_native(T value) {
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return __builtin_bswap32 (value);
    } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
        return  __builtin_bswap64 (value);
    }

    static_assert(sizeof(T) < sizeof(uint32_t) ||
        sizeof(T) > sizeof(uint64_t),
        "Endianness conversion is disabled for this type");
}

// Convert little endian data to native
// This is a no-op because we only support little endian for now
template <typename T>
T little_endian_to_native(T value) {
    return value;
}
