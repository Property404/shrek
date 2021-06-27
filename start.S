/* From https://wiki.osdev.org/ARMv7-A_Bare_Bones */
.global _start
_start:
    ldr sp, =STACK_TOP
    bl main
1:
    b 1b
.size _start, . - _start
