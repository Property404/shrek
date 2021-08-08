#include "common.h"
#include "drivers.h"
#include "io.h"
#include "machine_types.h"
#include "memory.h"
#include "serial.h"
#include <stdint.h>

extern uint32_t _heap_base;
extern uint32_t _heap_size;
extern "C" void boot(int machine_type, void* dtb) {
    // Set up heap allocator
    allocator.initialize(&_heap_base, reinterpret_cast<size_t>(&_heap_size));

    // TODO: Use device trees
    switch (machine_type) {
    case MACHINE_TYPE_VEXPRESS:
        pl011_uart_init((void*)0x1c090000);
        break;

    case MACHINE_TYPE_RPI:
        pl011_uart_init((void*)0x3F201000);
        break;

    default:
    case MACHINE_TYPE_QEMU_VIRT:
        pl011_uart_init((void*)0x09000000);
        break;
    }

    printf("Machine type: 0x%08x\n", machine_type);
    printf("DTB pointer : %p\n", dtb);
    printf("DTB magic   : 0x%08x\n", *(uint32_t*)dtb);
}
