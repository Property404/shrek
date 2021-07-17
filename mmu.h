#pragma once
// Map a physical device address to a virtual address
// and return said virtual address
void* remap_mmio(void* physical_device_address);

// Return true if MMU enabled, false otherwise
extern "C" int mmu_enabled();
