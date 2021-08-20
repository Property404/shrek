#include "memory.h"

extern uint32_t _heap_base;
extern uint32_t _heap_size;

Allocator allocator(&_heap_base, reinterpret_cast<size_t>(&_heap_size));
