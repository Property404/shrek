/*
   See ARM Cortex-A Series Programmer's Guide version 4.0, chapter 13 and chapter 9

   A lot of code was "liberated" from chapter 13.
   I believe chapter 13 discusses boot code from a bootloader's perspective, so
   we skip some initialization stuff

    Arguments:
    a1:  FDT pointer
*/

.global init_paging

init_paging:
    push {v1, v2, v3, lr}

    // Enable D-side prefetch
    // I don't really know what this is, but the programmer's guide recommend's it */
    // This is not a feature enabled on all Cortex A processors. That is, bit 2 is reserved.
    // However, I don't think it does anything harmful to write to it
    // https://community.arm.com/developer/ip-products/processors/f/cortex-a-forum/4815/d-side-prefetch-cortex-a8
    mrc p15, 0, v1, c1, c0, 1 // Read auxiliary control register
    orr v1, v1, #(0x1 <<2) // Enable D-side prefetch
    mcr p15, 0, v1, c1, c0, 1 // Write auxiliary control register
    dsb
    isb

    // a1 is already FDT
    ldr a2, =_kernel_virtual_base
    mov a3, sb 
    bl write_initial_page_tables;


    // Initialize MMU
    mov v1, #0x0 
    mcr p15, 0, v1, c2, c0, 2 // Write TTBCR
    // ttl1_base isn't fixed up by got_fixup
    ldr v2, =_kernel_virtual_base
    sub v2, v2, sb
    ldr v1, =_ttl1_base
    sub v1, v1, v2
    mcr p15, 0, v1, c2, c0, 0 // Write TTBR0

    // Map all domains to client
    // (legacy shit, nothing to see here)
    ldr v1, = 0x55555555
    mcr p15, 0, v1, c3, c0, 0 // Write DACR

    // Probably an unnecessary barrier 
    dsb
    isb

    // Enable MMU
    mrc p15, 0, v1, c1, c0, 0 // Read control register configuration data
    orr v1, v1, #0x01 // bit 0 is MMU enable
    mcr p15, 0, v1, c1, c0, 0 // Write Control Register configuration data

    // Probably an unnecessary barrier 
    dsb
    isb

    pop {v1, v2, v3, pc}


.size init_paging, . - init_paging
