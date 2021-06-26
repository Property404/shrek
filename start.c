/* From https://wiki.osdev.org/ARMv7-A_Bare_Bones */
#include <stdint.h>
#include <stddef.h>
#define UART0_BASE 0x1c090000

void puts(const char* buffer) {
    for(size_t i=0; buffer[i] != '\0';i++) {
        *(volatile uint32_t *)(UART0_BASE) = buffer[i];
    }
}

void nputs(const char* buffer, size_t len) {
    for(size_t i=0; i<len;i++) {
        char c = buffer[i];
        if ( c < 0x20 || c > 0x7f) {
            c = '.';
        }
        *(volatile uint32_t *)(UART0_BASE) = c;
    }
}

void start(void) {
    puts("Hello World!");
}
