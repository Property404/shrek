AS=arm-none-eabi-as
CC=arm-none-eabi-gcc

LDFLAGS=-T linker.ld 
CFLAGS=-march=armv7-a -ffreestanding -Wall -Wextra -fmax-errors=3
ASFLAGS=-march=armv7-a
OBJECTS= start.o main.o serial.o io.o console.o cmisc.o

build: kernel.elf
kernel.elf: $(OBJECTS)
	arm-none-eabi-ld $(LDFLAGS) $(OBJECTS) -o kernel.elf
clean:
	rm -f *.o
	rm -f *.elf
run: build
	qemu-system-arm -M vexpress-a15 -cpu cortex-a15 -kernel kernel.elf -nographic
