/* Derived from https://wiki.osdev.org/ARMv7-A_Bare_Bones */
ENTRY(_start)
 
SECTIONS {
    . = 0x00000000;

    _kernel_initial_base = .;
    _stack_size = 10000;
 
    .stub   : { start.o(.text) }
    .text   : { *(.text) }
    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .got.plt : { *(.got.plt) }
    .got    : {
        _got_start = .;
        *(.got);
        _got_end = .;
    }
    .bss    : { *(.bss COMMON) }
    .stack : {
        _stack_base = .;
        . += _stack_size;
        _stack_top = .;
    }
}
