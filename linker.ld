/* Derived from https://wiki.osdev.org/ARMv7-A_Bare_Bones */
ENTRY(_start)

SECTIONS {
    _kernel_virtual_base = 0x00010000;

    _vectors_virtual_base = 0x00000000;/*TODO: change to highvecs */

    /* For remapping MMIO to virtual mem */
    _mmio_map_base = _kernel_virtual_base + 0x10000000;
    _mmio_map_size = 0x10000000;

    _page_size = 0x1000;

    _stack_size = 10000;
    _ttl1_size = 4096*4;
    _ttl2_size = 256*4;
 
    . = _kernel_virtual_base;
    .stub   : {
        start.o(.text)
    }
    .text   : {
        EXCLUDE_FILE(*vectors.o) 
            *(.text) 
    }
    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .got.plt : { *(.got.plt) }
    .got    : {
        _got_start = .;
        *(.got);
        _got_end = .;
    }
    .bss    : {
        _bss_start = .;
        *(.bss COMMON)
        _bss_end = .;

    }
    .stack : ALIGN(16) {
        _stack_base = .;
        . += _stack_size;
        _stack_top = .;
    }
    .translation_table : ALIGN(_ttl1_size) {
        _ttl1_base = .;
        . += _ttl1_size;
        _ttl2_base = .;
        . += _ttl2_size * 4096;
    }
    /* This will be relocated to _vector_table_base */
    .vtext : ALIGN(_page_size){
        _vectors_initial_base = .;
        vectors.o(.text)
    }
    _kernel_virtual_end = .;
}
