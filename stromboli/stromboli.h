/* Simple kernel test framework for Shrek */

#pragma once
#include <functional>

#define REQUIRE_EQUALS(value1, value2) {if ((value1) != value2)\
    { printf("0x%x != 0x%x\n", (value1), (value2)); panic("Assertion failed");}}
#define REQUIRE(condition) {if (!(condition)) { panic("Assert failed: '" #condition );}}
#define REQUIRE_NOT(condition) REQUIRE(!(condition))

#define TEST_CASE(name, ...) static Stromboli::TestCase _puppet_##name\
    (Stromboli::extract_file_name(__FILE__), #name, []() __VA_ARGS__);

namespace Stromboli {

// https://stackoverflow.com/questions/31050113/
// how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
constexpr const char* extract_file_name(const char* path) {
    const char* file = path;
    while (*path) {
        if (*path++ == '/') {
            file = path;
        }
    }
    return file;
}

/// Stromboli test case.
class TestCase final {
    using Action = std::function<void()>;

    // The test procedure itself.
    Action* action = nullptr;

    // The name of the test case.
    char* name = nullptr;

 public:
    /// Define the test case.
    TestCase(const char* file_name, const char* name, Action action);

    /// Run the test case.
    void run();
};

/// Run all registered tests.
void runTests();

}  // namespace Stromboli
