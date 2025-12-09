#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

# Path to clang and compiler flags
CC=clang  # Ubuntu users: use CC=clang
CFLAGS="-std=c11 -O0 -g3 -Wall -Wextra --target=riscv32-unknown-elf -mno-relax -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib -Iinclude"

# Build the kernel
$CC $CFLAGS -Wl,-Tlinker.ld -Wl,-Map=kernel.map -o kernel.elf \
    start.S entry.S common.c scheduler.c main.c

# Start QEMU
$QEMU -machine virt -bios none -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf