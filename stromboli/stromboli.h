#pragma once
#include "../List.h"
#include <functional>

#define REQUIRE_EQUALS(value1, value2) {if ((value1) != value2)\
    { printf("0x%x != 0x%x\n", (value1), (value2)); _self->fail("Assertion failed");}}
#define REQUIRE(condition) {if (!(condition)) { _self->fail("Assert failed: '" #condition );}}
#define REQUIRE_NOT(condition) REQUIRE(!(condition))
#define REQUIRE_PANICS(expression) {_self->allowPanic();expression;_self->disallowPanic();}

#define TEST_CASE(name, action) static Stromboli::TestCase _puppet_##name ( #name, [](Stromboli::TestCase* _self) action );

namespace Stromboli {

class TestCase {
    using Action = std::function<void(Stromboli::TestCase*)>;

    Action* action = nullptr;
    const char* name = nullptr;

    
    public:
    TestCase(const char* name, Action action);

    void run();

    void fail(const char* message);

    // (Temporarily) allow panic
    void allowPanic();

    // Disallow panics again
    void disallowPanic();

    const char* getName() const {
        return name;
    }
};

/// Run all registered tests.
void runTests();
}
