// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2012-2014
 *     Texas Instruments Incorporated, <www.ti.com>
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2021
 * Dagan Martinez
 *
 * Derived from U-Boot source code
 */
#include "Driver.h"
#include <cstdint>
#include "common.h"
#include "drivers/timer.h"
#include "io.h"

constexpr uint32_t ARCH_TIMER_CTRL_ENABLE = 0x01;
constexpr uint64_t TIMER_COMP_VAL = 0xffffffffffffffffull;
constexpr uint32_t COUNTER_FREQUENCY = 0x3b9aca0;

class Armv7Timer final : public TimerDeviceCoupling {
    uint32_t hz = 0;

    static uint32_t read_cntfrq() {
        uint32_t frequency = 0;

        asm volatile("isb");
        asm volatile("dsb");
        asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (frequency));
        return frequency;
    }

 public:
     Armv7Timer(const DeviceNode* ) {
        // XXX: This halts pi-test for some reason
        // const uint32_t freq = COUNTER_FREQUENCY;
        // asm("mcr p15, 0, %0, c14, c0, 0" : : "r" (freq));
        // asm volatile("isb");
        // asm volatile("dsb");

        // Set PL1 Physical Timer Ctrl
        const uint32_t ctrl = ARCH_TIMER_CTRL_ENABLE;
        asm("mcr p15, 0, %0, c14, c2, 1" : : "r" (ctrl));
        asm volatile("isb");
        asm volatile("dsb");

        // Set PL1 Physical Comp Value
        const uint64_t val = TIMER_COMP_VAL;
        asm("mcrr p15, 2, %Q0, %R0, c14" : : "r" (val));
        asm volatile("isb");
        asm volatile("dsb");

        hz = read_cntfrq();
     }

     uint64_t getTicks() override {
        uint64_t now;

        asm("mrrc p15, 0, %Q0, %R0, c14" : "=r" (now));

        return now;
     }
};


REGISTER_DRIVER("arm,armv7-timer", Armv7Timer);
