loader:
	nasm -felf32 loader.asm -o loader.o

kernel:
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

link:
	i686-elf-gcc -T linker.ld -o kern.bin -ffreestanding -O2 -nostdlib loader.o kernel.o -lgcc

iso:
	mkdir -p isodir/boot/grub/
	cp -v kern.bin isodir/boot/
	grub-mkrescue -o g0s.iso isodir

clean:
	rm -r *.o *.bin *.iso  

kern_bin: loader kernel link

all: kern_bin iso
