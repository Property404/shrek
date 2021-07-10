#include "io.h"
#include "console.h"
#include <stdint.h>
extern uint32_t _ttl1_base;
void main(void) {
    console();

    puts("We're done here, folks!\n");
}
