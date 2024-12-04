#include <vrs_test.h>

/// base address of the control registers (if any)
#define CONTROL_BASE 0x1D000000ULL

// base address of the translated memory region
#define TRANSLATION_BASE 0xC0000000ULL

// base address of the direct access region
#define DIRECT_ACCESS_BASE 0x100000000ULL

// // the input addres to be mapped
#define INPUT_ADDR 0x0

// base addresses of the tables in memory
#define PML4_BASE 0x1000
#define PDPT_BASE 0x2000
#define PDIR_BASE 0x3000
#define PTABLE_BASE 0x4000

// the output address to be mapped
#define OUTPUT_ADDR 0x5000

// the size of the mapping
#define MAP_SIZE 0x1000

#define LARGE_OUTPUT_ADDR (2 * 1024 * 1024)
#define LARGE_MAP_SIZE (2 << 20)

#define HUGE_OUTPUT_ADDR (1014 * 1024 * 1024)
#define HUGE_MAP_SIZE (1 << 30

int vrs_test() {
    MSG("Running VRS tests for: x86_64_pagetable\n");

    volatile uint64_t *control = (uint64_t *)CONTROL_BASE;
    volatile uint32_t *control32 = (uint32_t *)CONTROL_BASE;
    volatile uint64_t *memory = (uint64_t *)TRANSLATION_BASE;

    // configure some mapping
    control[0] = PML4_BASE;
    control32[2] = (0x1ULL << 31);

    volatile uint64_t *pml4 = (uint64_t *)(DIRECT_ACCESS_BASE + PML4_BASE);
    pml4[0] = PDPT_BASE | 0x3;
    volatile uint64_t *pdpt = (uint64_t *)(DIRECT_ACCESS_BASE + PDPT_BASE);
    pdpt[0] = PDIR_BASE | 0x3;
    pdpt[1] = HUGE_OUTPUT_ADDR | 0x3 | (1 << 7);

    volatile uint64_t *pdir = (uint64_t *)(DIRECT_ACCESS_BASE + PDIR_BASE);
    pdir[0] = PTABLE_BASE | 0x3;
    pdir[1] = LARGE_OUTPUT_ADDR | 0x3 | (1 << 7);

    volatile uint64_t *ptable = (uint64_t *)(DIRECT_ACCESS_BASE + PTABLE_BASE);
    ptable[0] = OUTPUT_ADDR | 0x3;

    // write to the memory
    MSG("Writing memory\n");
    *memory = 0x1234;

    // verify
    volatile uint64_t *dca_memory = (uint64_t *)(DIRECT_ACCESS_BASE + OUTPUT_ADDR);

    MSG("Verifying..2.\n");
    if (*dca_memory != 0x1234) {
        MSG("Error: memory not mapped correctly\n");
        return 0;
    } else {
        MSG("Memory mapped correctly: %lx expected 0x1234\n", *dca_memory);
    }

    MSG("Velosiraptor tests completed.\n");
    return 1;
}
