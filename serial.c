#include "errno.h"
#include <stdint.h>
#include <stdbool.h>
/*
 * Currently only implementing for PL011 UART
 * http://static6.arrow.com/aropdfconversion/32f6a7175ece91477c63bc40811c02e077718861/ddi0183.pdf
 */
#define UART0_BASE 0x1c090000

void putchar(char c) {
    *(volatile uint32_t *)(UART0_BASE) = c;
}

bool testchar(void) {
    if ((*(volatile uint32_t *)(UART0_BASE+0x18)) & 0x10)
        return false;
    return true;
}

int getchar(void) {
    if ((*(volatile uint32_t *)(UART0_BASE+0x18)) & 0x10)
        return EAGAIN;

    const int data = (*(volatile uint32_t *)(UART0_BASE));
    if(data & 0xFFFFFF00) {
        *(volatile uint32_t *)(UART0_BASE + 0x04) = 0xFFFFFFFF;
        return -1;
    }
    return data;
}
