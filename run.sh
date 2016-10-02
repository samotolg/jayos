#!/bin/sh
qemu-system-arm -M versatilepb -cpu arm1176 -kernel main.elf -nographic
