/* Initially derived from
 * https://wiki.osdev.org/ARMv7-A_Bare_Bones */
#include "config.h"
.global _start
_start:
    /*
       Record the inital PC in r9
       Used to offset GOT and constants from linker
     */
    mov r9, pc
    sub r9, r9, #8

    /* Save off boot params to v4/v5
           Same conventions as Linux:
           r0 holds 0
           r1 holds the machine type
           r2 holds the DTB pointer
   */
    mov v4, r1
    mov v5, r2


    /* Enable NEON
     *
     * https://developer.arm.com/documentation/dui0472/i/compiler-coding-practices/enabling-neon-and-fpu-for-bare-metal
     */
#ifdef __ARM_NEON
    mrc p15,0,r0,c1,c0,2    // Read CP Access register
    orr r0,r0,#0x00f00000   // Enable full access to NEON/VFP (Coprocessors 10 and 11)
    mcr p15,0,r0,c1,c0,2    // Write CP Access register
    isb
    mov r0,#0x40000000      // Switch on the VFP and NEON hardware
    vmsr fpexc,r0            // Set EN bit in FPEXC
#endif

    /* Address linker expects to be loaded at, which is probably wrong */
    ldr v1, =_kernel_virtual_base

    /*
       Set up the stack
       Note that this is likely the wrong address, so we have to adjust it.
       Same goes for _got_end and _got_start
     */
    ldr sp, =_stack_top
    sub sp, sp, v1
    add sp, sp, r9

    /*
       Change the Global Offset Table

       We do this so we don't have to compile a different kernel for
       each load address
     */
    mov a1, v1
    mov a2, r9
    bl fixup_got

#ifdef CONFIG_EARLY_SERIAL
    mov a1, v4
    mov a2, v5
    bl boot
#endif

    // Enable D-side prefetch
    // I don't really know what this is, but the programmer's guide recommend's it */
    // This is not a feature enabled on all Cortex A processors. That is, bit 2 is reserved.
    // However, I don't think it does anything harmful to write to it
    // https://community.arm.com/developer/ip-products/processors/f/cortex-a-forum/4815/d-side-prefetch-cortex-a8
    mrc p15, 0, a2, c1, c0, 1 // Read auxiliary control register
    orr a2, a2, #(0x1 <<2) // Enable D-side prefetch
    mcr p15, 0, a2, c1, c0, 1 // Write auxiliary control register
    dsb
    isb

    // Enable high vecs
    mrc	p15, 0, r0, c1, c0, 0	/* V bit (bit[13]) in CP15 c1 */
    orr r0, #(1 << 13)
    mcr	p15, 0, r0, c1, c0, 0	/* V bit (bit[13]) in CP15 c1 */

    // Create page tables
    mov a1, v5
    ldr a2, =_kernel_virtual_base
    mov a3, sb 
    bl write_initial_page_tables;

    // Initialize MMU
    mov a2, #0x0 
    mcr p15, 0, a2, c2, c0, 2 // Write TTBCR
    // ttl1_base isn't fixed up by got_fixup
    ldr a3, =_kernel_virtual_base
    sub a3, a3, sb
    ldr a2, =_ttl1_base
    sub a2, a2, a3
    mcr p15, 0, a2, c2, c0, 0 // Write TTBR0

    // Map all domains to client
    // (legacy shit, nothing to see here)
    ldr a2, = 0x55555555
    mcr p15, 0, a2, c3, c0, 0 // Write DACR

    // Enable MMU
    mrc p15, 0, a2, c1, c0, 0 // Read control register configuration data
    orr a2, a2, #0x01 // bit 0 is MMU enable
    mcr p15, 0, a2, c1, c0, 0 // Write Control Register configuration data

    // Probably an unnecessary barrier 
    dsb
    isb

    // Fixup stack and PC
    ldr sp, =_stack_top
    ldr pc, =_pc_fixup_ret
    _pc_fixup_ret:

    // Restore GOT
    mov a1, r9
    mov a2, v1
    bl fixup_got

    // Fixup DTB pointer
    sub v5, v5, sb
    add v5, v5, v1

    // Zero out BSS
    ldr v1, =_bss_start
    ldr v2, =_bss_end
    mov r0, #0x0
    _bss_zero_loop:
        str r0, [v1]
        add v1, v1, #4
        cmp v1, v2
        bne _bss_zero_loop

    // Do system set up
    mov a1, v4
    mov a2, v5
    bl boot

    // This shouldn't return
    bl kmain

    /* But if it does, enter a low power state*/
_loop_forever:
    /*
       For whatever reason QEMU seems to lock up when using `wfi`?
       That is, you can't ^a+x to quit
       `wfe` works perfectly fine though
     */
    wfe
    b _loop_forever

.size _start, . - _start
