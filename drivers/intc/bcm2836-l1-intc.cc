// SPDX-License-Identifier: GPL-2.0+
// https://datasheets.raspberrypi.com/bcm2836/bcm2836-peripherals.pdf

#include "Driver.h"
#include <cstdint>
#include "common.h"
#include "drivers/intc.h"
#include "io.h"
#include "mmu.h"

constexpr uint32_t LOCAL_TIMER_CTRL_INT_ENABLE = 1 << 29;
constexpr uint32_t LOCAL_TIMER_CTRL_TIMER_ENABLE = 1 << 28;
constexpr uint32_t CNTPNSIRQ_IRQ_ENABLE = 1 << 1;

static inline unsigned get_cpsr(void) {
    unsigned long retval;
    asm volatile ("mrs %0, cpsr" : "=r" (retval) :);
    return retval;
}

static inline void set_cpsr(unsigned val) {
    asm volatile ("msr cpsr, %0":  :"r" (val));
}


typedef struct {
    uint32_t control_register;  // 0x00
    uint32_t _unused0;  // 0x04
    uint32_t core_timer_prescalar;  // 0x08
    uint32_t gpu_interrupts_routing;  // 0x0C
    uint32_t perf_mon_routing_set;  // 0x10
    uint32_t perf_mon_routing_clear;  // 0x14
    uint32_t _unused1;  // 0x18
    uint32_t core_timer_access_ls;  // 0x1C
    uint32_t core_timer_access_ms;  // 0x20
    uint32_t local_int0_routing;  // 0x24
    uint32_t _unused2;  // 0x28
    uint32_t axi_outstanding_counters;  // 0x2C
    uint32_t axi_outstanding_irqs;  // 0x30
    uint32_t local_timer_ctrl;  // 0x34
    uint32_t local_timer_write_flags;  // 0x38
    uint32_t _unused3;  // 0x40
    uint32_t core0_timers_int_ctrl;  // 0x44
} Intc;

class BcmLocalIntcDriver final : public InterruptControllerCoupling {
    volatile Intc* intc = nullptr;

 public:
     BcmLocalIntcDriver(const DeviceNode* node) {
        void* const physical_address = node->getBaseAddress();
        intc = (decltype(intc))remap_mmio(physical_address);

        intc->local_int0_routing = 0x00;
        intc->core0_timers_int_ctrl = CNTPNSIRQ_IRQ_ENABLE;

        // Enable timer
        intc->local_timer_ctrl |= LOCAL_TIMER_CTRL_INT_ENABLE | LOCAL_TIMER_CTRL_TIMER_ENABLE | 100;
        intc->local_timer_write_flags |= 0xC0000000;


        /*
        printf("CSPR: 0x%x\n", get_cpsr());
        uint32_t new_val =  get_cpsr() & ~(1u<<7u);
        printf("New Val: 0x%x\n", new_val);
        set_cpsr(new_val);
        asm("isb");
        asm("dsb");
        printf("CSPR: 0x%x\n", get_cpsr());
        */
     }
};


REGISTER_DRIVER("brcm,bcm2836-l1-intc", BcmLocalIntcDriver);
