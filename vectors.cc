#include <cstdint>
#include "panic.h"
#include "io.h"

enum class ExceptionType : uint32_t {
    FIQ = 0x1C,
    RESET = 0x00,
    ILLEGAL_INSTRUCTION = 0x04,
    SWI = 0x08,
    PREFETCH = 0x0C,
    DATA = 0x10,
    IRQ = 0x18
};

static void irq_handler() {
    printf("Oh boy it's an IRQ!\n");
}

extern "C" void exception_handler(ExceptionType exception_type) {
    if (exception_type == ExceptionType::IRQ) {
        irq_handler();
    } else {
        panic("Hey hey it's an exception of type 0x%x!", exception_type);
    }
}
