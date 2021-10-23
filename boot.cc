#include <cstdint>
#include <cinttypes>
#include "common.h"
#include "Driver.h"
#include "globals.h"
#include "io.h"
#include "machine_types.h"
#include "memory.h"

// Call C++ constructors
static void call_global_constructors();

// Load all drivers
static void load_drivers();

extern "C" void boot(int machine_type, void* dtb) {
    // This sets up the heap allocator
    call_global_constructors();

    globals.machine_type = static_cast<MachineType>(machine_type);
    globals.device_tree = allocator.construct<DeviceTree>(dtb);

    load_drivers();

    printf("Machine type: 0x%08x\n", machine_type);
    printf("DTB pointer : %p\n", dtb);
    printf("DTB magic   : 0x%08" PRIx32 "\n", *(uint32_t*)dtb);

    printf("Drivers:\n");
    for (const auto driver : *drivers) {
        printf("\t%s%s\n", driver->isLoaded()?"+":"-", driver->getCompatibleString());
    }
}

static void load_drivers() {
    for (const auto& node : *(globals.device_tree)) {
        for (const auto driver : *drivers) {
            driver->loadIfCompatible(&node);
        }
    }
}

static void call_global_constructors() {
    using Func = void(*)(void);

    extern Func _init_array_start[0], _init_array_end[0];

    for (Func* constructor = _init_array_start; constructor != _init_array_end; constructor++) {
        (*constructor)();
    }
}
