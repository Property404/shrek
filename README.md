# Shrek

A kernel for ARMv7. This is very much a work in progress. No user space yet!

## Building

```bash
# Choose whichever armv7 triple you want
export CROSS_COMPILE=arm-none-eabi-
make
```

## Running on Qemu

```bash
make pi # Test on QEMU's Raspberry Pi 2 vm
make virt # Test on QEMU's "virt" machine
make vexpress # Test on QEMU's vexpress-a15
```

## Debugging

You can append `-debug` to a QEMU target.
Note that the following can only be used to debug AFTER paging has been set up.

```bash
make pi-debug

# In another terminal
arm-none-eabi-gdb kernel.elf
> target remote localhost:1234
```

## Running unit tests

You can append `-test` after a QEMU target:

```bash
make pi-test
```

And you can lint with:

```bash
make lint
```

## Configuration

You can edit the definitions in `config.h`. This is
ignored by git, so you don't have to worry about
accidentally committing those changes.

## License

GPLv2, as some code from U-Boot is used.
