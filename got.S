.global fixup_got

/* 
   Change the Global Offset Table

   We do this so we don't have to compile a different kernel for
   each load address

   C Signature:
       void fixup_got(void* current_base, void* desired_base)

   Arguments:
       R0/a1 - Current GOT base address
       R1/a2 - Desired GOT base address

   THIS REQUIRES THE STACK POINTER TO BE GOOD
   (this requirement might need to be changed later,
   since we can't easily fix up the stack from C)
*/
fixup_got:
    push {v1, v2, v3, lr}

    /*
       _got_end and _got_start are always
       based off _kernel_virtual_base

       If _kernel_virtual_base, fixing up these two values
       are a no-op
    */
    ldr v3, =_kernel_virtual_base

    ldr v1, =_got_end
    sub v1, v1, v3
    add v1, v1, a2

    ldr v2,=_got_start // ptr
    sub v2, v2, v3
    add v2, v2, a2
    _got_adjust_loop:

        // Update entry
        ldr v3, [v2]//v3=*ptr
        sub v3, v3, a1//v3 -= current_got_base
        add v3, v3, a2//v3 += desired_got_base
        str v3, [v2]//*ptr = v3

        // Next entry
        add v2, v2, #4

        cmp v2, v1
        bne _got_adjust_loop

    pop {v1, v2, v3, pc}
