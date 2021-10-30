/* Simple kernel test framework for Shrek */

#pragma once
#include <functional>
#include <cstdio>
#include <cinttypes>
#include "panic.h"

#define REQUIRE_EQUALS(value1, value2) do {\
    Stromboli::_REQUIRE_EQUALS<std::remove_reference_t<decltype(value1)>, \
    std::remove_reference_t<decltype(value2)>>(""#value1, ""#value2, value1, value2);}while(0)
#define REQUIRE(condition) {if (!(condition)) { panic("Assert failed: '" #condition );}}
#define REQUIRE_NOT(condition) REQUIRE(!(condition))

#define TEST_CASE(name, ...) static Stromboli::TestCase _puppet_##name\
    (Stromboli::extract_file_name(__FILE__), #name, []() __VA_ARGS__);

namespace Stromboli {

// Statically fail an `if constexpr` branch
static void fail_at_compile_time(const char* message) {
    puts(message);
}

template<class T, class U>
constexpr void _REQUIRE_EQUALS(const char* a, const char* b, T v1, U v2) {
    if constexpr (sizeof(v1) != sizeof(v2)) {
        panic("Stromboli: Cannot compare values of different sizes (%s vs %s)", a, b);
    }
    if constexpr (std::is_signed_v<T> != std::is_signed_v<U>) {
        panic("Stromboli: Cannot compare signed and unsigned\n"
                "\t`%s` is %ssigned\n"
                "\t`%s` is %ssigned"
                , a, std::is_signed_v<T>?"":"un"
                , b, std::is_signed_v<U>?"":"un");
    }
    if ((v1) != v2) {
        if constexpr (sizeof(v1) == sizeof(uint32_t)) {
            printf("0x%" PRIx32 " != 0x%" PRIx32 "\n",
                    (uint32_t)(v1), (uint32_t)(v2));
        } else if (sizeof(v1) == sizeof(uint32_t)) {
            printf("0x%" PRIx64 " != 0x%" PRIx64 "\n",
                    (uint64_t)(v1), (uint64_t)(v2));
        } else {
            fail_at_compile_time("_REQUIRE_EQUALS not equipped to handle item of this size");
        }
        panic("Assertion failed: `%s` == `%s`", a, b);
    }
}

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
