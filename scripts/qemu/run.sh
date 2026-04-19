#!/bin/bash
qemu-system-i386 -kernel build/blackos.elf -m 256M -serial stdio -vga std
