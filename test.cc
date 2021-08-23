#include "common.h"
#include "io.h"
#include "globals.h"
#include "List.h"
#include "DeviceTree.h"
#include "stromboli/stromboli.h"

TEST_CASE(alignment_test, {
    REQUIRE(ALIGNED_TO(0x100, 0x100));
    REQUIRE(ALIGNED_TO(0x1000, 0x100));
    REQUIRE(ALIGNED_TO(0x851000, 0x100));
    REQUIRE(ALIGNED_TO(0x200, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x11, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x10, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0xFF, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x180, 0x100));
});

TEST_CASE(device_tree_test, {
    const void* dtb = globals.device_tree->getBlobAddress();
    DeviceTree dt(dtb);
});

TEST_CASE(list_test, {
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

    REQUIRE_PANICS(list.get(3));
    list.get(4);
});

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
