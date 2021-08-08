#include "io.h"
void halt() {
    while(true) {
        asm volatile ("wfi");
    }
}

void panic(const char* fmt,...) {
    // Let's provide the option to not print anything
    // if serial isn't set up
    if (fmt != nullptr) {
        puts("PANIC! ");

        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        putchar('\n');
    }

    halt();
}
