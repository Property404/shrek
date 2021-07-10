.global vector_table
.section .text
vector_table:
b reset_handler
b undefined_handler
b swi_handler
b prefetch_handler
b data_handler
nop //reserved
b irq_handler

/* FIQ handler begins here */
mov v5, #0x10
b generic_handler

reset_handler:
    mov v5, #0x11
    b generic_handler

undefined_handler:
    mov v5, #0x12
    b generic_handler

swi_handler:
    mov v5, #0x13
    b generic_handler

prefetch_handler:
    mov v5, #0x14
    b generic_handler

data_handler:
    mov v5, #0x15
    b generic_handler

irq_handler:
    mov v5, #0x16
    b generic_handler

generic_handler:
_loop_forever:
    /*
       For whatever reason QEMU seems to lock up when using `wfi`?
       That is, you can't ^a+x to quit
       `wfe` works perfectly fine though
     */
    wfe
    b _loop_forever
