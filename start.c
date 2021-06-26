/* From https://wiki.osdev.org/ARMv7-A_Bare_Bones */
#include <stdint.h>
#define UART0_BASE 0x1c090000
 
void start() {
    *(volatile uint32_t *)(UART0_BASE) = 'A';
}
