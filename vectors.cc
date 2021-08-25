#include <cstdint>
#include "panic.h"

enum class ExceptionType : uint32_t {
    FIQ = 0x00,
    RESET,
    ILLEGAL_INSTRUCTION,
    SWI,
    PREFETCH,
    DATA,
    IRQ
};

extern "C" void exception_handler(ExceptionType exception_type) {
    panic("Hey hey it's an exception of type %x!", exception_type);
}
