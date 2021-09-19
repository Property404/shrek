#include "stromboli/stromboli.h"
#include "common.h"

TEST_CASE(alignment, {
    REQUIRE(ALIGNED_TO(0x100, 0x100));
    REQUIRE(ALIGNED_TO(0x1000, 0x100));
    REQUIRE(ALIGNED_TO(0x851000, 0x100));
    REQUIRE(ALIGNED_TO(0x200, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x11, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x10, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0xFF, 0x100));
    REQUIRE_NOT(ALIGNED_TO(0x180, 0x100));
});
