; Declare constants for the multiboot header.
MBALIGN   equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO   equ  1 << 1            ; provide memory map
FLAGS     equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC     equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM  equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
STACKSIZE equ  (32 * 1024)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16

; Setup stack here in bss for 32 KB
stack_bottom:
resb STACKSIZE
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
    mov esp, $stack_top         ; setup stack 

    extern kernel_main
    call kernel_main            ; drop in to the kernel in C
    cli
    .hang:                      ; loop infinitely here and do nothing
        hlt
        jmp .hang
    .end:

