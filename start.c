/* From https://wiki.osdev.org/ARMv7-A_Bare_Bones */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "io.h"
#include "console.h"

void start(void) {
    console();

    puts("We're done here, folks!\n");
}
