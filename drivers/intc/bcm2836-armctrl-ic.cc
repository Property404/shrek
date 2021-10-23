// SPDX-License-Identifier: MIT
// Based on https://github.com/BrianSidebotham/arm-tutorial-rpi/tree/master/part-4
// Copyright @BrianSidebotham

#include "Driver.h"
#include <cstdint>
#include "common.h"
#include "drivers/intc.h"
#include "io.h"
#include "mmu.h"

/** @brief The interrupt controller memory mapped register set */
typedef struct {
    volatile uint32_t IRQ_basic_pending;
    volatile uint32_t IRQ_pending_1;
    volatile uint32_t IRQ_pending_2;
    volatile uint32_t FIQ_control;
    volatile uint32_t Enable_IRQs_1;
    volatile uint32_t Enable_IRQs_2;
    volatile uint32_t Enable_Basic_IRQs;
    volatile uint32_t Disable_IRQs_1;
    volatile uint32_t Disable_IRQs_2;
    volatile uint32_t Disable_Basic_IRQs;
} BcmIntc;

class BcmIntcDriver final : public InterruptControllerCoupling {
    volatile BcmIntc* intc = nullptr;

 public:
     BcmIntcDriver(const DeviceNode* node) {
        void* const physical_address = node->getBaseAddress();
        intc = (decltype(intc))remap_mmio(physical_address);

        // Enable timer
        intc->Enable_Basic_IRQs = 0x01;
     }
};


// REGISTER_DRIVER("brcm,bcm2836-armctrl-ic", BcmIntcDriver);
