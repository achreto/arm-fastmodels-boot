#include <vrs_test.h>

/// base address of the control registers (if any)
#define CONTROL_BASE 0x1D000000ULL

// base address of the translated memory region
#define TRANSLATION_BASE 0xC0000000ULL

// base address of the direct access region
#define DIRECT_ACCESS_BASE 0x100000000ULL

// // the input addres to be mapped
#define INPUT_ADDR 0x0

// the output address to be mapped
#define OUTPUT_ADDR 0x1000

// the size of the mapping
#define MAP_SIZE 0x1000

int vrs_test() {
    MSG("Running VRS tests for: assoc_segment\n");

    volatile uint64_t *control = (uint64_t *)CONTROL_BASE;
    volatile uint64_t *memory = (uint64_t *)TRANSLATION_BASE;

    // configure some mapping
    control[0] = OUTPUT_ADDR;
    control[1] = INPUT_ADDR;
    control[2] = MAP_SIZE;
    control[3] = 0x3;

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
    return 0;
}
