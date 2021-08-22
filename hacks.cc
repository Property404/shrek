#include <functional>
#include "panic.h"
// This file includes miscellaneous hacks to get a C++ environment
// working.

namespace std {
    [[noreturn]] void __throw_bad_function_call() {
        panic("Bad function call");
        halt();
    }
}
