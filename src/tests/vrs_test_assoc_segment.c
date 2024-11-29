#include <vrs_test.h>

/// base address of the control registers (if any)
#define CONTROL_BASE 0x1D000000ULL

// base address of the translated memory region
#define MEMORY_BASE 0xC0000000ULL

// base address of the direct access region
#define DIRECT_ACCESS_BASE 0x1C0000000ULL

int vrs_test() {
    MSG("Running VRS tests for: assoc_segment\n");

    volatile uint64_t *control = (uint64_t *)CONTROL_BASE;
    volatile uint64_t *memory = (uint64_t *)MEMORY_BASE;

    // write to the memory
    *memory = 0x1234;

    MSG("Velosiraptor tests completed.\n");
    return 1;
}
