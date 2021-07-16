CROSS_COMPILE?=arm-none-eabi-
AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy
QEMU=qemu-system-arm

LDFLAGS=-T linker.ld -g
CFLAGS=-march=armv7-a -ffreestanding -Wall -Wextra -fmax-errors=1 -I include -Og -g3 -fpie -fpic
CXXFLAGS=$(CFLAGS) -fno-exceptions
ASFLAGS=-march=armv7-a -g3 -fpie -fpic
CPP_DTC_FLAGS=-x assembler-with-cpp -nostdinc -I dts/include/ -D__ASSEMBLY__ -undef -D__DTS__

OBJECTS = mmu.o mmu_asm.o start.o main.o serial.o io.o console.o cmisc.o boot.o pl011_uart.o got.o vectors.o

all: kernel.bin
config.h: 
	touch config.h
kernel.elf: config.h *.h $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf
kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@
%.dtb: dts/%.dts
	cpp $(CPP_DTC_FLAGS) $< | dtc -O dtb -o $@
clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.dtb
	rm -f *.bin
vexpress: kernel.bin
	$(QEMU) -s -machine vexpress-a15 -cpu cortex-a15 -kernel kernel.bin -serial mon:stdio -nographic
virt: kernel.bin 
	$(QEMU) -s -machine virt   -cpu cortex-a7 -kernel kernel.bin -serial mon:stdio -nographic

# Note - this doesn't work yet
pi: kernel.bin bcm2836-rpi-2-b.dtb
	$(QEMU) -s -machine raspi2 -cpu cortex-a7 -kernel kernel.bin -serial mon:stdio -nographic -dtb bcm2836-rpi-2-b.dtb
