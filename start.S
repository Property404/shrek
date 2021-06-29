/* From https://wiki.osdev.org/ARMv7-A_Bare_Bones */
.global _start
_start:
    /*
       Record the inital PC in r9
       Used to offset GOT and constants from linker
     */
    mov r9, pc
    sub r9, r9, #8

    /* Address linker expects to be loaded at, which is probably wrong */
    ldr r4, =_kernel_initial_base

    /*
       Set up the stack
       Note that this is likely the wrong address, so we have to adjust it.
       Same goes for _got_end and _got_start
     */
    ldr sp, =_stack_top
    sub sp, sp, r4
    add sp, sp, r9

    /*
       Change the Global Offset Table

       We do this so we don't have to compile a different kernel for
       each load address
     */

    ldr r5, =_got_end
    sub r5, r5, r4
    add r5, r5, r9

    ldr r6,=_got_start // ptr
    sub r6, r6, r4
    add r6, r6, r9
    _got_adjust_loop:

        // Update entry
        ldr r8, [r6]//r8=*ptr
        sub r8, r8, r4//r8 -= _kernel_initial_base
        add r8, r8, r9//r8 += initial_pc
        str r8, [r6]//*ptr = r8

        add r6, r6, #4
        cmp r6, r5
    bne _got_adjust_loop


    /* Same conventions as Linux:
       r0 holds 0
       r1 holds the machine type
       r2 holds the DTB pointer
   */
    mov r0, r1
    mov r1, r2

    /* This shouldn't return */
    bl boot

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
