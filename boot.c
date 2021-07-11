#include "io.h"
#include "serial.h"
#include "drivers.h"
#include <stdint.h>
#include "machine_types.h"
int main(void);

void boot(int machine_type, void* dtb) {

    // TODO: Use device trees
    switch (machine_type) {
    case MACHINE_TYPE_VEXPRESS:
        pl011_uart_init((void*)0x1c090000);
        break;
    case MACHINE_TYPE_QEMU_VIRT:
    default:
        pl011_uart_init((void*)0x09000000);
        break;
    }

    printf("Machine type: %x\n", machine_type);
    printf("DTB pointer : %x\n", dtb);
    printf("DTB magic   : %x\n", *(uint32_t*)dtb);
}
