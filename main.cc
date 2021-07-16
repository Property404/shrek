#include "io.h"
#include "console.h"
#include <stdint.h>
uint32_t _ttl1_base;
extern "C" void kmain(void) {
    console();

    puts("We're done here, folks!\n");
}
