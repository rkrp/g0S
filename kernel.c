#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "Need a ix86 cross compiler to build this kernel"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This needs to be compiled with a 32 bit ix86-elf compiler"
#endif

#define VGA_WIDTH 80
#define VGA_HEIGHT 30

size_t term_x; 
size_t term_y;
uint16_t* term_buffer;
 
/* Hardware text mode color constants. */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

/* 4 bits bg color + 4 bits fg color */
inline uint8_t vga_entry_color(enum vga_color bg, enum vga_color fg) {
    return bg << 4 | fg;
}

inline uint16_t get_color_char(uint8_t chr, uint8_t term_color) {
    return term_color << 8 | chr ;
}

void terminal_initialize() {
    term_x = 0;
    term_y = 0;
    term_buffer = (uint16_t *) 0xb8000;
}

inline void write_char_at(size_t x, size_t y, uint8_t chr, uint8_t term_color) {
    size_t index = y * VGA_WIDTH + x;
    if(chr == '\n') {
        term_y++;
        term_x = 0;
        return;
    }
    term_buffer[index] = get_color_char(chr, term_color);
}

void kern_puts(const char *str) {
    uint8_t term_color; 
    term_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY);
    
    for(uint8_t *chr = (uint8_t *) str ; *chr ; chr++) {
        if(term_x < VGA_WIDTH)
            write_char_at(term_x++, term_y, *chr, term_color);
    }
}

void kernel_main() {
    terminal_initialize();
    kern_puts("Hello from kernel mode!\nHello again :)");
}
