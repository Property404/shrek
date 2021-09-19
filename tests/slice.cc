#include "Slice.h"
#include "stromboli/stromboli.h"

TEST_CASE(basic, {
    int array[] = {3, 1, 4, 1, 5, 9, 2};
    Slice slice(array, sizeof(array)/sizeof(*array));

    REQUIRE_EQUALS(slice.size(), sizeof(array)/sizeof(*array));

    size_t i = 0;
    for (const auto& item : slice) {
        REQUIRE_EQUALS(array[i], slice[i]);
        REQUIRE_EQUALS(item, array[i]);
        i++;
    }

    REQUIRE_EQUALS(i, slice.size());
});

static std::function<uint32_t(uint32_t)> borg = big_endian_to_native<uint32_t>;

TEST_CASE(scalar_transform, {
    uint32_t array[] = {0xBADDECAF, 0xDEADBABE, 0x11111111, 0x12345678};

    Slice<uint32_t, &borg>
        slice(array, sizeof(array)/sizeof(*array));

    REQUIRE_EQUALS(slice.size(), sizeof(array)/sizeof(*array));

    size_t i = 0;
    for (const auto& item : slice) {
        REQUIRE_EQUALS(slice[i], big_endian_to_native(array[i]));
        REQUIRE_EQUALS(item, slice[i]);
        i++;
    }

    REQUIRE_EQUALS(i, slice.size());
});
