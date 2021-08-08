CROSS_COMPILE?=arm-none-eabi-

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy
QEMU=qemu-system-arm

COMMON_GCC_FLAGS=-march=armv7-a -ffreestanding -Wall -Wextra -fmax-errors=1 -I\
	include -Og -g3 -fpic $(DEFINES)
LDFLAGS=-T linker.ld.processed -g
CFLAGS=$(COMMON_GCC_FLAGS) -std=c11
CXXFLAGS=$(COMMON_GCC_FLAGS) -fno-exceptions -std=c++17
ASFLAGS=-march=armv7-a -g3 -fpie -fpic
CPP_DTC_FLAGS=-x assembler-with-cpp -nostdinc -I dts/include/\
	  -D__ASSEMBLY__ -undef -D__DTS__
QEMU_FLAGS=-kernel $(EXECUTABLE_NAME).bin -serial mon:stdio -nographic 

OBJECTS = memory.o Allocator.o mmu.o mmu_asm.o start.o main.o serial.o io.o console.o cmisc.o boot.o pl011_uart.o got.o vectors.o panic.o

ifeq ($(findstring -debug,$(MAKECMDGOALS)),-debug)
	QEMU_FLAGS+=-S
endif
ifeq ($(findstring -test,$(MAKECMDGOALS)),-test)
	OBJECTS:=$(filter-out main.o,$(OBJECTS))
	OBJECTS+=test.o
	EXECUTABLE_NAME=kernel.test
	QEMU_FLAGS+=-semihosting
else
	EXECUTABLE_NAME=kernel
endif

all: $(EXECUTABLE_NAME).bin
config.h: 
	touch config.h
linker.ld.processed:
	 cpp $(DEFINES) linker.ld | grep -v '^#' > linker.ld.processed 
%.dtb: dts/%.dts
	cpp $(CPP_DTC_FLAGS) $< | dtc -O dtb -o $@
$(EXECUTABLE_NAME).elf: linker.ld.processed config.h *.h $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE_NAME).elf
$(EXECUTABLE_NAME).bin: $(EXECUTABLE_NAME).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.dtb
	rm -f *.bin
	rm -f *.processed

# Run unit tests in qemu
# Eg: `make pi-test`
%-test:  % ;

# Way to go into debug mode for QEMU targets
# Example: `make pi-debug` or `make virt-debug`
%-debug: % ;

# QEMU targets
vexpress: $(EXECUTABLE_NAME).bin
	$(QEMU) -s -machine vexpress-a15 -cpu cortex-a15 $(QEMU_FLAGS)
virt: $(EXECUTABLE_NAME).bin 
	$(QEMU) -s -machine virt   -cpu cortex-a7 $(QEMU_FLAGS)
pi: $(EXECUTABLE_NAME).bin
	$(QEMU) -s -machine raspi2 -cpu cortex-a7  $(QEMU_FLAGS)
