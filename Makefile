CC=i686-elf-gcc
OPTS=-O2
INCLUDES=-Iutil/includes
CFLAGS=-std=gnu99 -ffreestanding -Wall -Wextra $(OPTS) $(INCLUDES)

src="kernel.c"
objs="kernel.o"

loader:
	nasm -felf32 loader.asm -o loader.o

kernel:
	$(CC) $(CFLAGS) -c -o kernel.o kernel.c

utils:
	$(CC) $(CFLAGS) -c -o util/string.o util/string.c

link:
	$(CC) -T linker.ld -o kern.bin -ffreestanding -O2 -nostdlib loader.o kernel.o util/string.o -lgcc

iso:
	mkdir -p isodir/boot/grub/
	cp -v kern.bin isodir/boot/
	grub-mkrescue -o g0s.iso isodir

clean:
	rm -r *.o *.bin *.iso  

kern_bin: loader utils kernel link

all: kern_bin iso
