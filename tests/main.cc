#include "stromboli/stromboli.h"
#include "common.h"
#include "io.h"

void quit_qemu() {
    printf("Quitting QEMU...\n");
    asm volatile("mov r0, #0x18");
    asm volatile("ldr r1, =0x20026");
    asm volatile("dsb");
    asm volatile("svc 0x00123456");  // make semihosting call
}

extern "C" void kmain() {
    printf("Starting tests\n");

    Stromboli::runTests();

    printf("Test suite successful!\n");
    quit_qemu();
}
