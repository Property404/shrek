.global mmu_enabled

/* Check if MMU is enabled

   Returns true if MMU enabled, otherwise false
*/
mmu_enabled:
    push {v1, lr}
    mrc p15, 0, a1, c1, c0, 0 // Read control register configuration data
    and a1, a1, #0x01 // bit 0 is MMU enable
    pop {v1, pc}
