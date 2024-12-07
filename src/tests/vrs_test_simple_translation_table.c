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

//
#define PTABLE_BASE 0x1000

// the output address to be mapped
#define OUTPUT_ADDR 0x4000

// the size of the mapping
#define MAP_SIZE 0x1000


int vrs_test() {
    MSG("Running VRS tests for: simple_translation_table\n");


    volatile uint32_t *control = (uint32_t *)CONTROL_BASE;
    volatile uint64_t *memory = (uint64_t *)TRANSLATION_BASE;

    // configure some mapping
    MSG("Reconfigure..\n");
    control[0] = PTABLE_BASE;
    volatile uint32_t *ptable = (uint32_t *)(DIRECT_ACCESS_BASE + PTABLE_BASE);
    ptable[0] = OUTPUT_ADDR | 0x3;

    // write to the memory
    MSG("Writing memory\n");
    for (size_t i = 0; i < MAP_SIZE / sizeof(*memory); i++) {
        printf("Writing %p\n", &memory[i]);
        memory[i] = i;
    }


    MSG("Reconfigure..\n");
    ptable[0] = (2 * OUTPUT_ADDR) | 0x3;

    MSG("Writing memory..\n");
    for (size_t i = 0; i < MAP_SIZE / sizeof(*memory); i++) {
        memory[i] = 2 * i;
    }

    // verify
    volatile uint64_t *dca_memory = (uint64_t *)(DIRECT_ACCESS_BASE + OUTPUT_ADDR);

    bool errors = false;
    MSG("Verifying memory...\n");
    for (size_t i = 0; i <  MAP_SIZE / sizeof(*memory); i++) {
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
