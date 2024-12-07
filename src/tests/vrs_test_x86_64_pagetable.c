#include <vrs_test.h>
#include<string.h>

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
    MSG("Reconfigure..\n");
    control[0] = PML4_BASE;
    control32[2] = (0x1ULL << 31);

    volatile uint64_t *pml4 = (uint64_t *)(DIRECT_ACCESS_BASE + PML4_BASE);
    bzero((void *)pml4, 4096);
    pml4[0] = PDPT_BASE | 0x3;

    volatile uint64_t *pdpt = (uint64_t *)(DIRECT_ACCESS_BASE + PDPT_BASE);
    bzero((void *)pdpt, 4096);
    pdpt[0] = PDIR_BASE | 0x3;
    pdpt[1] = HUGE_OUTPUT_ADDR | 0x3 | (1 << 7);

    volatile uint64_t *pdir = (uint64_t *)(DIRECT_ACCESS_BASE + PDIR_BASE);
    bzero((void *)pdir, 4096);
    pdir[0] = PTABLE_BASE | 0x3;
    pdir[1] = LARGE_OUTPUT_ADDR | 0x3 | (1 << 7);

    volatile uint64_t *ptable = (uint64_t *)(DIRECT_ACCESS_BASE + PTABLE_BASE);
    bzero((void *)ptable, 4096);
    ptable[0] = OUTPUT_ADDR | 0x3;

    // write to the memory
    MSG("Writing memory\n");
    for (size_t i = 0; i < MAP_SIZE / sizeof(*memory); i++) {
        memory[i] = i;
    }

    MSG("Reconfigure..\n");
    ptable[0] = (2 * OUTPUT_ADDR) | 0x3;

    // write some more memory
    MSG("Writing memory..\n");
    for (size_t i = 0; i < MAP_SIZE / sizeof(*memory); i++) {
        memory[i] = 2 * i;
    }

    // verify
    volatile uint64_t *dca_memory = (uint64_t *)(DIRECT_ACCESS_BASE + OUTPUT_ADDR);

    bool errors = false;
    MSG("Verifying memory...\n");
    for (size_t i = 0; i < MAP_SIZE / sizeof(*memory); i++) {
        if (dca_memory[i] != i) {
            WARN("Memory[%zu] not mapped correctly: %lx expected %lx\n", i, dca_memory[i], i);
            errors = true;
        }
    }

    MSG("Verifying memory...\n");
    dca_memory = (uint64_t *)(DIRECT_ACCESS_BASE + 2 * OUTPUT_ADDR);
    for (size_t i = 0; i <  MAP_SIZE / sizeof(*memory); i++) {
        if (dca_memory[i] != 2 * i) {
            WARN("Memory[%zu] mapped correctly: %lx expected %lx\n", i, dca_memory[i], 2 * i);
            errors = true;
        }
    }

    if (errors) {
        ERROR("Errors found in memory mapping\n");
    } else {
        MSG("All memory mapped correctly\n");
    }

    MSG("Velosiraptor tests completed.\n");
    return 1;
}
