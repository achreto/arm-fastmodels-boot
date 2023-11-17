#include <types.h>
#include <config.h>
#include <bootloader_args.h>
#include <serial.h>
#include <stdio.h>

#include <logging.h>
#include <armv8.h>
#include <processor.h>
#include <exceptions.h>
#include <mmu.h>
#include <cachectrl.h>

/* velosiraptor MMU testing */
/* implementation is provided by the specific MMU */
int vrs_test(void);
