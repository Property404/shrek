#include "stromboli/stromboli.h"
#include "List.h"

TEST_CASE(basic, {
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

    // Make sure we can pop an element
    REQUIRE_EQUALS(list.pop_front(), 3);
    REQUIRE_EQUALS(list.get(0), 1);
    REQUIRE_EQUALS(list.get(1), 4);
    REQUIRE_EQUALS(list.size(), 2);
});

TEST_CASE(popping, {
    const int numbers[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
    constexpr auto size = sizeof(numbers)/sizeof(numbers[0]);
    List<int> list;

    for (int i=0; i < 4; i++) {
        REQUIRE_EQUALS(list.size(), 0);

        for (int number : numbers) {
            list.push_back(number);
            REQUIRE_EQUALS(list.back(), number);
            REQUIRE_EQUALS(list.front(), numbers[0]);
        }

        for (size_t j = 0; j < size; j++) {
            REQUIRE_EQUALS(list.back(), numbers[size - 1]);
            REQUIRE_EQUALS(list.pop_front(), numbers[j]);
            if (j != size - 1) {
                REQUIRE_EQUALS(list.front(), numbers[j+1]);
            }
            REQUIRE_EQUALS(list.size(), size - j - 1)
        }
    }
});
