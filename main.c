/* From https://wiki.osdev.org/ARMv7-A_Bare_Bones */
#include "io.h"
#include "console.h"

void main(void) {
    console();

    puts("We're done here, folks!\n");
}
