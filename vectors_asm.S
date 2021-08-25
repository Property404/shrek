.section .text
vector_table:
// 0x00 - Reset
b reset_handler
// 0x04 - Illegal instruction
b undefined_handler
// 0x08 - Software Interrupt(syscall)
b swi_handler
// 0x0C - Instruction fetch
b prefetch_handler
// 0x10 - Mem fetch
b data_handler
// 0x14 - Reserved - not used for anything
nop
// 0x18 - Interrupt
b irq_handler

/* 0x1C - FIQ handler begins here */
mov a1, #0x0
b generic_handler

reset_handler:
    mov a1, #0x1
    b generic_handler

undefined_handler:
    mov a1, #0x2
    b generic_handler

swi_handler:
    mov a1, #0x3
    b generic_handler

prefetch_handler:
    mov a1, #0x4
    b generic_handler

data_handler:
    mov a1, #0x5
    b generic_handler

irq_handler:
    mov a1, #0x6
    b generic_handler

generic_handler:
    ldr pc,=exception_handler
