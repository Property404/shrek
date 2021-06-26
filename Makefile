all: build
build:
	arm-none-eabi-as -march=armv7-a -mcpu=cortex-a15 _start.S -o _start.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -c start.c -o start.o
	arm-none-eabi-ld -T linker.ld _start.o start.o -o kernel.elf
clean:
	rm *.o
	rm *.elf
run: build
	qemu-system-arm -M vexpress-a15 -cpu cortex-a15 -kernel kernel.elf -nographic
