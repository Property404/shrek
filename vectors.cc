#include <cstdint>
#include "panic.h"
#include "io.h"

enum class ExceptionType : uint32_t {
    FIQ = 0x00,
    RESET ,
    ILLEGAL_INSTRUCTION,
    SWI,
    PREFETCH,
    DATA,
    IRQ
};

static void irq_handler() {
    printf("Oh boy it's an IRQ!\n");
}

extern "C" void exception_handler(ExceptionType exception_type) {
    if (exception_type == ExceptionType::IRQ) {
        irq_handler();
    } else {
        panic("Hey hey it's an exception of type %x!", exception_type);
    }
}
