#include "stromboli/stromboli.h"
#include "DeviceTree.h"
#include "globals.h"
#include "cmisc.h"

TEST_CASE(basic_access, {
    const void* dtb = globals.device_tree->getBlobAddress();
    DeviceTree dt(dtb);

    // Make sure we can iterate through our own device tree
    for (const auto& node : dt) {
        (void)node;
    }
});

TEST_CASE(parsing, {
    extern void* sample_device_tree;
    DeviceTree dt(sample_device_tree);

    const auto& root = dt.getRoot();

    REQUIRE(!strcmp(root.getProperty("model").getValueAsString(), "LEGO MINDSTORMS EV3"));
    REQUIRE_EQUALS(root.getProperty("#size-cells").getValueAsScalar<uint32_t>(), 1u);
    REQUIRE_EQUALS(root.getProperty("#address-cells").getValueAsScalar<uint32_t>(), 1u);

    const auto& memory_reg = root.getNode("memory").getProperty("reg").getValueAsSlice<uint32_t>();
    REQUIRE_EQUALS(memory_reg[0], 0xc0000000);
    REQUIRE_EQUALS(memory_reg[1], 0x04000000u);

    REQUIRE_EQUALS(root.getNode("memory").getBaseAddress(), (void*)(0xc0000000));
    REQUIRE_EQUALS(root.getNode("mmc@40000").getBaseAddress(), (void*)(0x01c40000));
});
