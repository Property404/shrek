#include "stromboli.h"
#include "../memory.h"
#include "../io.h"

namespace Stromboli {

List<TestCase*>* test_cases = nullptr;

void runTests() {
    for (const auto& test_case : *test_cases) {
        test_case->run();
    }
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

    (*action)();

    const auto allocations_at_end = allocator.getNumberOfAllocations();

    if (allocations_at_start != allocations_at_end) {
        panic("%s: A memory leak occurred while running the test.", name);
    }
}

}
