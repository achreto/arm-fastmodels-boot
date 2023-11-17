#
# Arm FastModels Hello World Boot
#
# Copyright 2020 Reto Achermann
# SPDX-License-Identifier: GPL-3.0
#

IMGNAME=bootimg

# the fastmodels simgen binary, for running the
SIMGEN := $(shell PATH=$(PATH) which simgen)

#
# Setting up the tools to be used for compilation and linking
#

# The C Compiler to be used
CC=aarch64-linux-gnu-gcc

CPP=aarch64-linux-gnu-cpp

OBJCOPY=aarch64-linux-gnu-objcopy

OBJDUMP=aarch64-linux-gnu-objdump

#
# Compiler flags for linking and compiling
#

BUILDFLAGS =\
	-O2 -g -Wall -Wextra -std=c17 -Werror \
	-fno-builtin \
	-fno-unwind-tables \
	-fno-stack-check -ffreestanding -fomit-frame-pointer \
	-nostdinc \
	-std=c99 \
	-mcpu=cortex-a57 \
	-march=armv8-a+nofp

LDFLAGS=\
	-O2 -g -Wl,-N -pie \
	-fno-builtin -nostdlib \
	-Wl,--fatal-warnings -Wl,--build-id=none

INC=-I./src/include

CFILES=src/boot.c src/string.c src/processor.c src/printf.c src/serial.c src/exceptions.c \
       src/mmu.c src/cachectrl.c src/ptable.c

SFILES=src/entry.S src/exception_vectors.S

OBJS=$(CFILES:%.o=%.c) $(SFILES:%.o=%.S)

ifndef VRS_TEST
	CFILES += src/vrs_test.c
else
	CFILES += $(VRS_TEST)
endif

#
# build target
#

all: $(IMGNAME).elf $(IMGNAME).bin

#
# cleanup
#
clean:
	rm -rf build $(IMGNAME).elf $(IMGNAME).bin

#
# Compiling single files
#

build:
	mkdir -p build


build/%.o : src/%.S build
	$(CC) $(BUILDFLAGS) $(INC) -o $@ -c $<


build/%.o : src/%.c build
	$(CC) $(BUILDFLAGS) $(INC) -o $@ -c $<


# this generates the linker script for the bootimage
build/$(IMGNAME).lds : src/boot.lds.in build
	$(CPP) $(INC) -D__ASSEMBLER__ -P src/boot.lds.in $@


#
# Building the boot image
#

# This is the bootloader elf with all debug symbols etc included
build/$(IMGNAME).full : $(OBJS) build/$(IMGNAME).lds
	$(CC) $(LDFLAGS)  $(INC) -T./build/$(IMGNAME).lds -Wl,-Map,./build/$(IMGNAME).map -o $@ $(OBJS)
	bash -c "echo -e '\0002'" | dd of=$@ bs=1 seek=16 count=1 conv=notrunc status=noxfer 2>&1


# This is the debug information of the bootloader elf
build/$(IMGNAME).debug : build/$(IMGNAME).full
	$(OBJCOPY) --only-keep-debug ./build/$(IMGNAME).full $@


# This is the assembly dump of the bootloader
build/$(IMGNAME).asm : build/$(IMGNAME).elf
	$(OBJDUMP) -D build/$(IMGNAME).elf > $@


# this is the bootimage elf
build/$(IMGNAME).elf : build/$(IMGNAME).full build/$(IMGNAME).debug
	$(OBJCOPY) -g --add-gnu-debuglink=./build/$(IMGNAME).debug ./build/$(IMGNAME).full $@

# this is the boot image elf
$(IMGNAME).elf : build/$(IMGNAME).elf build/$(IMGNAME).asm
	cp build/$(IMGNAME).elf $(IMGNAME).elf

# this is the boot image binary
$(IMGNAME).bin : $(IMGNAME).elf
	$(OBJCOPY) -O binary $(IMGNAME).elf $(IMGNAME).bin



build/platforms/armv8_minimal/isim_system : platforms/armv8_minimal/ARMv8_Minimal.lisa  platforms/armv8_minimal/ARMv8_Minimal.sgproj
ifndef SIMGEN
	$(error "No 'simgen' in PATH, please source 'FAST_MODELS_ROOT/source_all.sh'")
endif
	$(SIMGEN) --num-comps-file 50 --gen-sysgen --warnings-as-errors \
		   --build-directory ./build/platforms/armv8_minimal \
		   -p platforms/armv8_minimal/ARMv8_Minimal.sgproj -b

run_armv8_minimal: $(IMGNAME).bin build/platforms/armv8_minimal/isim_system
	build/platforms/armv8_minimal/isim_system --data Memory0=$(IMGNAME).bin@0x0

run_armv8_minimal_debug: $(IMGNAME).bin build/platforms/armv8_minimal/isim_system
	bash ./tools/run_debugger.sh $(IMGNAME).bin $(IMGNAME).elf
