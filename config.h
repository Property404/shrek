#pragma once
/*
 * Use this to add custom configuration to your Shrek build
 * Changes to this file are not tracked by git
 */

// Enable this if you want to use printf() before
// page tables are set up
#undef CONFIG_EARLY_SERIAL

// Where the kernel is located in virtual memory
#define CONFIG_KERNEL_VIRTUAL_BASE 0x10000
