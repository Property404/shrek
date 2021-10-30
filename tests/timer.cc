#include "stromboli/stromboli.h"
#include "drivers/timer.h"

TEST_CASE(monotonic_check, {
    auto coupling = TimerDeviceCoupling::getInstance();


    auto last = coupling->getTicks();

    for (int step = 0; step < 10; step++) {
        unsigned i = 0;
        for (i = 0; i < 1000000; i++) {
            asm volatile("nop");
        }

        auto current = coupling->getTicks();
        REQUIRE(current > last);
        last = current;
    }
});
