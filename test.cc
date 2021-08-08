#include "common.h"
#include "io.h"
#include "List.h"

#define REQUIRE_EQUALS(value1, value2) {if ((value1) != value2) { printf("%x != %x\n", (value1), (value2));panic("Assertion failed");}}
#define REQUIRE(condition) {if (!(condition)) { panic("Assert failed: '" #condition );}}
#define REQUIRE_NOT(condition) REQUIRE(!(condition))

static void alignment_test() {
    REQUIRE(ALIGNED_TO(0x100, 0x100));
    REQUIRE(ALIGNED_TO(0x1000, 0x100));
    REQUIRE(ALIGNED_TO(0x851000, 0x100));
    REQUIRE(ALIGNED_TO(0x200, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x11, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x10, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0xFF, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x180, 0x100));
}

static void list_test() {
    List<int> list;
    REQUIRE(list.size() == 0);

    list.push_back(3);
    REQUIRE_EQUALS(list.get(0), 3);
    REQUIRE_EQUALS(list.size(), 1);

    list.push_back(1);
    REQUIRE_EQUALS(list.get(1), 1);
    REQUIRE_EQUALS(list.size(), 2);

    list.push_back(4);
    REQUIRE_EQUALS(list.get(2), 4);
    REQUIRE_EQUALS(list.size(), 3);
}

extern "C" void kmain() {
    printf("Starting tests\n");

    alignment_test();
    list_test();


    printf("Tests finished!\n");
    halt();
}
