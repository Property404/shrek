#pragma once
#include "DeviceTree.h"
#include "machine_types.h"

// Let's try to keep all our major globals in one place.
// Ideally, the number of globals will be kept to a minimum.

/// Shrek's analog to U-Boot's gd_t.
struct _GlobalMonstrosity {
    /// Basic machine type, as passed by the boot loader.
    MachineType machine_type;
    /// Pre-parsed device tree.
    const DeviceTree* device_tree;
};

/// Global data structure. Shrek's analog to U-Boot's gd
extern _GlobalMonstrosity globals;
