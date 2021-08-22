#include <stdint.h>
#include "common.h"
#include "drivers.h"
#include "globals.h"
#include "io.h"
#include "machine_types.h"
#include "memory.h"
#include "serial.h"

// Call C++ constructors
static void call_global_constructors();

extern "C" void boot(int machine_type, void* dtb) {
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

    // This sets up the heap allocator
    call_global_constructors();

    globals.machine_type = static_cast<MachineType>(machine_type);
    globals.device_tree = allocator.construct<DeviceTree>(dtb);
}

static void call_global_constructors() {
    using Func = void(*)(void);

    extern Func _init_array_start[0], _init_array_end[0];

    for (Func* constructor = _init_array_start; constructor != _init_array_end; constructor++) {
        (*constructor)();
    }
}
