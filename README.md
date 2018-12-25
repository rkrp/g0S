g0s
===

My n-th attempt at writing a minimal x86 operating system.

Nothing fancy to see here.

Build
-----

Just run 

    make kern_bin iso

You should see g0s.iso in the current directory

Run
---

It can be run in `qemu` by

    qemu-system-i386 -cdrom g0s.iso

Or if you fancy running it in a real x86 hardware, write it to a pendrive by

    sudo dd if=g0s.iso of=/dev/sdX && sync

Todo
----

 - [X] Add newline support in kern_puts()
 - [X] Scroll on filling the space allocated for terminal
 - [X] Add a method to print hex values.
 - [ ] Add logging to serial port for debugging
 - [ ] Decide on an executable format
 - [ ] Implement a dynamic memory allocator
