# Arm FastModels Hello World Boot
A simple, helloworld boot application for the Arm FastModels simulator.

## Authors

Reto Achermann

# License

See LICENSE file.

# Building

`make run_armv8_minimal`

# Velosiraptor tests

This is for plugging bootimg.bin into the hardware generator of the velosiraptor project.

`make VRS_TEST="../yourfile.c"`

where `yourfile.c` should implement the function `int vrs_test(void)`. This function may set up the
page table structure in a certain address range that does not trigger translation logic (currently
0x1C0000000 to 0x1ffffffff) and then test the translation logic by accessing another address range
(currently 0x00000000 to 0x0fffffff). Check the Platform.lisa file for the true address ranges.
