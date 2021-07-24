#include "io.h"
#include "serial.h"
#include "common.h"
#include "drivers.h"
#include <stdint.h>
#include "machine_types.h"
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

    printf("Machine type: %x\n", machine_type);
    printf("DTB pointer : %x\n", dtb);
    printf("DTB magic   : %x\n", *(uint32_t*)dtb);
}
