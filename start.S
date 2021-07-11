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

    // Init page tables
    bl init_paging

    // Restore GOT
    mov a1, r9
    mov a2, v1
    bl fixup_got

    // Fixup stack and PC
    ldr sp, =_stack_top
    ldr pc, =_pc_fixup_ret
    _pc_fixup_ret:

    // Fixup DTB pointer
    sub v5, v5, sb
    //ldr v1, = _kernel_virtual_base
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

    // This shouldn't return
    mov a1, v4
    mov a2, v5
    bl boot
    bl main

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
