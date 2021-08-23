#include "stromboli.h"
#include "../memory.h"
#include "../io.h"

namespace Stromboli {

static List<TestCase*>* test_cases = nullptr;
static size_t test_case_index = 0;
static bool tests_are_running = false;
static bool panic_allowed = false;
static int panics = 0;

static void panic(const char* fmt, ...) {
    puts("PANIC! ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');

    halt();
}

void runTests() {
    tests_are_running = true;

    for(test_case_index = 0; test_case_index < test_cases->size(); test_case_index++) {
        test_cases->get(test_case_index)->run();
_return_from_panic:
        continue;
    }

    tests_are_running = false;
}

void TestCase::fail(const char* message) {
    printf("stromboli: <%s> failed: %s\n", name, message);
    halt();
}

void TestCase::allowPanic() {
    panics = 0;
    panic_allowed = true;
}

void TestCase::disallowPanic() {
    if (panics == 0) {
        fail("Expected panic");
    }

    panics = 0;
    panic_allowed = false;
}


TestCase::TestCase(const char* name, Action func) {
    if (test_cases == nullptr) {
        test_cases = allocator.allocate<List<TestCase*>>(sizeof(*test_cases));
    }

    this->name = name;
    this->action = allocator.allocate<Action>(sizeof(func));
    *action = func;

    test_cases->push_back(this);
}

void TestCase::run() {
    printf("Running <%s>\n", name);
    const auto allocations_at_start = allocator.getNumberOfAllocations();

    (*action)(this);

    const auto allocations_at_end = allocator.getNumberOfAllocations();

    if (allocations_at_start != allocations_at_end) {
        panic("%s: A memory leak occurred while running the test.", name);
    }
}

}

// Override the default panic() so we can perform tests that expect them
void panic(const char* fmt, ...) {
    printf("~~panicking~~\n");
    Stromboli::panics++;

    if (!Stromboli::panic_allowed) {
        if (Stromboli::tests_are_running) {
            printf("stromboli: <%s> panicked: ", Stromboli::test_cases->get(Stromboli::test_case_index)->getName());
        }

        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        putchar('\n');

        halt();
    }
}

