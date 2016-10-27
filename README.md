#gOS

My n-th attempt at writing a minimal operating system.

Nothing fancy to see here.

##Build

`nasm` can be used to build this.

    nasm -f bin loader.asm -o loader.bin

##Run

It can be run in `qemu` by

    qemu-system-i386 -net none -fda loader.bin
