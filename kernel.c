#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
#if defined(__linux__)
#error "Need a ix86 cross compiler to build this kernel"
#endif
 
#if !defined(__i386__)
#error "This needs to be compiled with a 32 bit ix86-elf compiler"
#endif

#include "string.h"

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

    // Handle new line character
    // Must go to next line even if the str exceeds the width
    if(chr == '\n') {
        term_y++;
        term_x = 0;
        return;
    }

    // No-op if the string len exceeds the width of the screen 
    // and it is not a new line char
    if(term_x >= VGA_WIDTH)
        return;
    
    term_buffer[index] = get_color_char(chr, term_color);
}

void kern_puts(const char *str) {
    uint8_t term_color; 
    term_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_CYAN);
    
    for(uint8_t *chr = (uint8_t *) str ; *chr ; chr++) {
        write_char_at(term_x++, term_y, *chr, term_color);
    }
}

inline char digit_to_char(uint32_t digit) {
    return 0x30 + digit;
}

void int_to_str(int32_t num, char *buffer) {
    char* curr_char = buffer;

    if(num == 0) {
        kern_puts("0");
        return;
    }

    if(num < 0) {
        kern_strcpy(buffer, "-");
        curr_char++;
        num *= -1;
    }

    uint32_t num_len = 0;
    int32_t temp_num = num;
    while(temp_num) {
        num_len++;
        temp_num /= 10;
    }
    
    curr_char += num_len - 1;
    while(num) {
        *curr_char = digit_to_char(num % 10);
        num /= 10;
        curr_char--;
    }
}

void kern_print_int(int32_t num) {
    /*
     * 10 bytes for decimal max byte representation 
     * +1 byte for sign and 1 byte for null byte
     */
    char buffer[12] = {0};
    int_to_str(num, buffer);
    kern_puts(buffer);
}

void kernel_main() {
    terminal_initialize();
    kern_puts("Hello from kernel mode!\nHello again :)\n");
    kern_print_int(-123);
}
