/* Simple kernel test framework for Shrek */

#pragma once
#include <functional>
#include "../List.h"

#define REQUIRE_EQUALS(value1, value2) {if ((value1) != value2)\
    { printf("0x%x != 0x%x\n", (value1), (value2)); panic("Assertion failed");}}
#define REQUIRE(condition) {if (!(condition)) { panic("Assert failed: '" #condition );}}
#define REQUIRE_NOT(condition) REQUIRE(!(condition))

#define TEST_CASE(name, action) static Stromboli::TestCase _puppet_##name (#name, []() action);

namespace Stromboli {

/// Stromboli test case.
class TestCase {
    using Action = std::function<void()>;

    // The test procedure itself.
    Action* action = nullptr;

    // The name of the test case.
    const char* name = nullptr;

 public:
    /// Define the test case.
    TestCase(const char* name, Action action);

    /// Run the test case.
    void run();
};

/// Run all registered tests.
void runTests();

}  // namespace Stromboli
