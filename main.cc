#include <stdint.h>

#include "console.h"
#include "io.h"

extern "C" void kmain(void) {
    console();

    puts("We're done here, folks!\n");
}
