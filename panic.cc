#include "io.h"
void halt() {
    while(true) {
        asm volatile ("wfi");
    }
}

void panic(const char* fmt) {
    // Let's provide the option to not print anything
    // if serial isn't set up
    if(fmt) {
        puts("PANIC! ");
        puts(fmt);
        putchar('\n');
    }
    halt();
}
