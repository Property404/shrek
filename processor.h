#pragma once

// ARM architecture-specific functions

/// Get Current Program Status Register
extern "C" uint32_t get_cpsr();
/// Set Current Program Status Register
extern "C" void set_cpsr(uint32_t val);
/// Globally enable interrupts
extern "C" void enable_interrupts();
/// Globally disable interrupts
extern "C" void disable_interrupts();
