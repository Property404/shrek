#pragma once
#include "../List.h"
#include <functional>

#define REQUIRE_EQUALS(value1, value2) {if ((value1) != value2)\
    { printf("0x%x != 0x%x\n", (value1), (value2)); panic("Assertion failed");}}
#define REQUIRE(condition) {if (!(condition)) { panic("Assert failed: '" #condition );}}
#define REQUIRE_NOT(condition) REQUIRE(!(condition))

#define TEST_CASE(name, action) static Stromboli::TestCase _puppet_##name ( #name, []() action );

namespace Stromboli {

class TestCase {
    using Action = std::function<void()>;

    Action* action;
    const char* name;
    
    public:
    TestCase(const char* name, Action action);

    void run();
};

/// Run all registered tests.
void runTests();
}
