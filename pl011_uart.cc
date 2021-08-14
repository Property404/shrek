/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2003, 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
 *
 * (C) Copyright 2021
 * Dagan Martinez
 * 
 * Reference:
 * http://static6.arrow.com/aropdfconversion/32f6a7175ece91477c63bc40811c02e077718861/ddi0183.pdf
 */

#include <stdint.h>
#include <stdlib.h>

#include "drivers.h"
#include "errno.h"
#include "mmu.h"
#include "serial.h"

// Copy-pasted from U-Boot
static volatile struct {
    uint32_t    dr;        /* 0x00 Data register */
    uint32_t    ecr;        /* 0x04 Error clear register (Write) */
    uint32_t    pl010_lcrh;    /* 0x08 Line control register, high byte */
    uint32_t    pl010_lcrm;    /* 0x0C Line control register, middle byte */
    uint32_t    pl010_lcrl;    /* 0x10 Line control register, low byte */
    uint32_t    pl010_cr;    /* 0x14 Control register */
    uint32_t    fr;        /* 0x18 Flag register (Read only) */
#ifdef CONFIG_PL011_SERIAL_RLCR
    uint32_t    pl011_rlcr;    /* 0x1c Receive line control register */
#else
    uint32_t    reserved;
#endif
    uint32_t    ilpr;        /* 0x20 IrDA low-power counter register */
    uint32_t    pl011_ibrd;    /* 0x24 Integer baud rate register */
    uint32_t    pl011_fbrd;    /* 0x28 Fractional baud rate register */
    uint32_t    pl011_lcrh;    /* 0x2C Line control register */
    uint32_t    pl011_cr;    /* 0x30 Control register */
} *uart = NULL;


static void pl011_putchar(char c) {
    uart->dr = c;
}

static bool pl011_testchar(void) {
    if (uart->fr & 0x010)
        return false;
    return true;
}

static int pl011_getchar(void) {
    if (uart->fr & 0x010)
        return EAGAIN;

    const int data = (*(volatile uint32_t *)(uart));
    if (data & 0xFFFFFF00) {
        // Clear errors
        uart->ecr = 0xFFFFFFFF;
        return -1;
    }
    return data;
}

#include "io.h"
void pl011_uart_init(void* address) {
    uart = (decltype(uart))remap_mmio(address);
    serial_driver.getchar = pl011_getchar;
    serial_driver.putchar = pl011_putchar;
    serial_driver.testchar = pl011_testchar;
}
