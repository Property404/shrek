#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "io.h"
#define KB (1024)
#define MB (KB*KB)
#define TTL1_NUM_ENTRIES 4096
#define TTL2_NUM_ENTRIES 256
#define PAGE_SIZE (4*KB)
#define SECTOR_SIZE (1*MB)

extern uint32_t _ttl1_base;
extern uint32_t _ttl2_base;
extern uint32_t _kernel_virtual_base;
extern uint32_t _kernel_size;

/* Write page table entry to TTL1 */
static void write_ttl1_pte(int index, uint32_t entry) {
    uint32_t* ttl1_base = &_ttl1_base;
    uint32_t* pte_pointer = ttl1_base + index;
    *pte_pointer = entry;
}

static void map_sector(uintptr_t virtual, uintptr_t physical, uint32_t attributes) {
    write_ttl1_pte(virtual >> 20, physical | attributes);
}

static void map_l2desc(uintptr_t virtual, uint32_t attributes) {
    const uint32_t ttl2_base = (uint32_t)(&_ttl2_base);
    const int index = virtual>>20;
    write_ttl1_pte(index, (ttl2_base + TTL2_NUM_ENTRIES*index*4) |  attributes);
}

/* Write page table entry to TTL2 */
static void write_ttl2_pte(int index1, int index2, uint32_t entry) {
    uint32_t* ttl2_base = &_ttl2_base;
    uint32_t* pte_pointer = ttl2_base + TTL2_NUM_ENTRIES*index1 + index2;
    *pte_pointer = entry;
}

static void map_page(uintptr_t virtual, uintptr_t physical, uint32_t attributes) {
    // TTL1 index is the top 12 bits
    const int index = virtual >> 20;

    // TTL2 index is the next 8 bits
    const int index2 = (virtual & 0xFF000)>>12;

    write_ttl2_pte(index, index2, physical | attributes);
}

void write_initial_page_tables(uint32_t initial_pc, uint32_t vbase) {
    // Full access, strongly ordered, global
    const uint32_t sector_attributes = 0x00000DE2;
    const uint32_t l2desc_attributes = 0x000001E1;
    const uint32_t page_attributes =   0x00000032;

    // Flat mapping(TODO: don't do this)
    for(unsigned i=0; i < 0x1000; i++) {
        map_sector(i<<20, i<<20, sector_attributes);
    }

    // Map vector table
    map_l2desc(0, l2desc_attributes);
    map_page(0, 0, page_attributes);

    // Map kernel --assuming size is <= 1MB
    const uint32_t kernel_virtual_base = vbase;
    const size_t kernel_size = (uint32_t)(&_kernel_size)-(initial_pc-vbase);
    for(size_t i=0;i<kernel_size;i+=SECTOR_SIZE) {
        map_l2desc(kernel_virtual_base+i, l2desc_attributes);
    }
    for(size_t i=0;i<kernel_size;i+=PAGE_SIZE) {
        map_page(kernel_virtual_base+i, initial_pc+i, page_attributes);
    }
}
