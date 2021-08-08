#include "common.h"
#include "io.h"

#define REQUIRE(condition, message) {if (!(condition)) { panic("Assert failed: '" #condition "'\n\t" message);}}
#define REQUIRE_NOT(condition, message) REQUIRE(!(condition), message)


extern "C" void kmain() {
    printf("Starting tests\n");
    REQUIRE(0==0, "Zero not equal zero!");

    REQUIRE(ALIGNED_TO(0x100, 0x100), "ALIGNED_TO failure");
    REQUIRE(ALIGNED_TO(0x1000, 0x100), "ALIGNED_TO failure");
    REQUIRE(ALIGNED_TO(0x851000, 0x100), "ALIGNED_TO failure");
    REQUIRE(ALIGNED_TO(0x200, 0x100), "ALIGNED_TO failure");
    REQUIRE_NOT(ALIGNED_TO(0x11, 0x100), "ALIGNED_TO failure");
    REQUIRE_NOT(ALIGNED_TO(0x10, 0x100), "ALIGNED_TO failure");
    REQUIRE_NOT(ALIGNED_TO(0xFF, 0x100), "ALIGNED_TO failure");
    REQUIRE_NOT(ALIGNED_TO(0x180, 0x100), "ALIGNED_TO failure");

    printf("Tests finished!");
    halt();
}
