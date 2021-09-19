#include "stromboli.h"
#include "../List.h"
#include "../memory.h"
#include "../io.h"
#include "../cmisc.h"

namespace Stromboli {

List<TestCase*>* test_cases = nullptr;

void runTests() {
    for (const auto& test_case : *test_cases) {
        test_case->run();
    }
}

TestCase::TestCase(const char* file_name, const char* name, Action func) {
    if (test_cases == nullptr) {
        test_cases = allocator.allocate<List<TestCase*>>(sizeof(*test_cases));
    }

    this->name = allocator.allocate<char>(strlen(file_name) + strlen(name));

    // Display the name in namespace::test_name format
    size_t index;
    for (index = 0; file_name[index] != '.'; index++) {
        this->name[index] = file_name[index];
    }
    this->name[index++] = ':';
    this->name[index++] = ':';
    memcpy(this->name + index, name, strlen(name) + 1);  // Include nullptr

    this->action = allocator.allocate<Action>(sizeof(func));
    *action = func;

    test_cases->push_back(this);
}

void TestCase::run() {
    printf("Running test <%s>\n", this->name);

    // Primitive memory leak detection.
    const auto allocations_at_start = allocator.getNumberOfAllocations();

    (*action)();

    const auto allocations_at_end = allocator.getNumberOfAllocations();

    if (allocations_at_start != allocations_at_end) {
        panic("%s: A memory leak occurred while running the test.", name);
    }
}

}  // namespace Stromboli
