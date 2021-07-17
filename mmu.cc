#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#define KB (1024)
#define MB (KB*KB)
#define TTL1_NUM_ENTRIES 4096
#define TTL2_NUM_ENTRIES 256
#define PAGE_SIZE (4*KB)
#define SECTOR_SIZE (1*MB)
#define FDT_SIZE 0x10000

// These aren't GOT-affected.
extern uint32_t _ttl1_base;
extern uint32_t _ttl2_base;
extern uint32_t _ram_virtual_base;
extern uint32_t _kernel_virtual_base;
extern uint32_t _kernel_virtual_end;
extern uint32_t _vectors_virtual_base;
extern uint32_t _vectors_initial_base;

/* Write page table entry to TTL1 */
static void write_ttl1_pte(int index, uint32_t entry) {
    uint32_t* ttl1_base = &_ttl1_base;
    uint32_t* pte_pointer = ttl1_base + index;
    *pte_pointer = entry;
}

static void map_sector(uintptr_t virt, uintptr_t physical, uint32_t attributes) {
    write_ttl1_pte(virt>> 20, physical | attributes);
}

static void map_l2desc(uintptr_t virt, uint32_t attributes) {
    const uint32_t ttl2_base = (uint32_t)(&_ttl2_base);
    const int index = virt>>20;
    write_ttl1_pte(index, (ttl2_base + TTL2_NUM_ENTRIES*index*4) |  attributes);
}

/* Write page table entry to TTL2 */
static void write_ttl2_pte(int index1, int index2, uint32_t entry) {
    uint32_t* ttl2_base = &_ttl2_base;
    uint32_t* pte_pointer = ttl2_base + TTL2_NUM_ENTRIES*index1 + index2;
    *pte_pointer = entry;
}

static void map_page(uintptr_t virt, uintptr_t physical, uint32_t attributes) {
    // TTL1 index is the top 12 bits
    const int index = virt>> 20;

    // TTL2 index is the next 8 bits
    const int index2 = (virt& 0xFF000)>>12;

    write_ttl2_pte(index, index2, physical | attributes);
}

static void map_region_by_page(uintptr_t virt, uintptr_t physical, size_t size, uint32_t page_attributes) {
    const uint32_t l2desc_attributes = 0x000001E1;
    for(size_t i=0;i<size;i+=SECTOR_SIZE) {
        map_l2desc(virt+i, l2desc_attributes);
    }
    for(size_t i=0;i<size;i+=PAGE_SIZE) {
        map_page(virt+i, physical+i, page_attributes);
    }
}

extern "C" void write_initial_page_tables(
        uint32_t fdt_base,// true physical address
        uint32_t kernel_base,// true virtual address
        uint32_t initial_pc// physical address
)
{
    const ptrdiff_t delusion =  (initial_pc-kernel_base);

    // Full access, strongly ordered, global
    const uint32_t sector_attributes_device = 0x00000DE2;
    const uint32_t page_attributes_device   = 0x00000032;

    // Normal, shareable, cacheable
    const uint32_t page_attributes_normal   = 0x0000043E;

    // Flat mapping(TODO: don't do this)
    for(unsigned i=1; i < 0x1000; i++) {
        map_sector(i<<20, i<<20, sector_attributes_device);
    }

    // Map vector table
    const uint32_t vectors_virtual_base = ((uint32_t)&_vectors_virtual_base) - delusion;
    const uint32_t vectors_physical_base = (uintptr_t)(&_vectors_initial_base);
    map_region_by_page(vectors_virtual_base, vectors_physical_base, PAGE_SIZE, page_attributes_normal);

    // Map code (kernel + preceding RAM)
    size_t ksize = (uintptr_t)(&_kernel_virtual_end)-(uintptr_t)(&_kernel_virtual_base);
    ksize = ALIGN_UP(ksize, SECTOR_SIZE);
    map_region_by_page(kernel_base, initial_pc, ksize, page_attributes_normal);

    // Map FDT
    map_region_by_page(ALIGN_DOWN(fdt_base - delusion, PAGE_SIZE), ALIGN_DOWN(fdt_base, PAGE_SIZE), 0x10000, page_attributes_normal);
}
