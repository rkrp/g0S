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
#include "mem.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

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
    kern_memset(term_buffer, 0, VGA_HEIGHT * VGA_WIDTH * 2);
}

inline uint32_t terminal_2d_to_index(int x, int y) {
    return y * VGA_WIDTH + x;
}
inline uint16_t* terminal_2d_to_addr(int x, int y) {
    return &term_buffer[y * VGA_WIDTH + x];
}

void term_scroll_up() {
    for(int row = 1 ; row < VGA_HEIGHT ; row++) {
        void *src = terminal_2d_to_addr(0, row);
        void *dest = terminal_2d_to_addr(0, row - 1);
        kern_memcpy(dest, src, VGA_WIDTH * sizeof(*term_buffer));
    }
    void *last_row = terminal_2d_to_addr(0, VGA_HEIGHT - 1);
    kern_memset(last_row, 0, VGA_WIDTH * sizeof(*term_buffer));
    term_x = 0;
    term_y = VGA_HEIGHT - 1;
}

inline void term_handle_newline() {
    if(term_y + 1 >= VGA_HEIGHT) {
        term_scroll_up();
    } else {
        term_y++;
        term_x = 0;
    }
}

inline void write_char_at(size_t x, size_t y, uint8_t chr, uint8_t term_color) {
    size_t index = terminal_2d_to_index(x, y);

    // Handle new line character
    // Must go to next line even if the str exceeds the width
    if(chr == '\n') {
        term_handle_newline();
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

inline char digit_to_hexchar(uint32_t digit) {
    if(digit < 10)
        return digit_to_char(digit);
    switch(digit) {
        case 10:
            return 'a';
        case 11:
            return 'b';
        case 12:
            return 'c';
        case 13:
            return 'd';
        case 14:
            return 'e';
        case 15:
            return 'f';
        default:
            //Ideally this should never be returned
            return 'T';
    } 
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

/* 
 * buffer should be at least of 10 bytes in size. 
 * Caller should make sure of it
 */

void int32_to_hex(int32_t num, char *buffer) {
    uint8_t shift = 0;
    uint8_t i = 10;
    buffer[i--] = '\x00';
    do {
        uint8_t byte = (num >> shift) & 0xff;
        uint8_t low_nib = byte & 0xf;
        uint8_t high_nib = (byte >> 4) & 0xf;

        buffer[i--] = digit_to_hexchar(low_nib);
        buffer[i--] = digit_to_hexchar(high_nib);
        shift += 8; 
    } while (i != 1);
    buffer[i--] = 'x';
    buffer[i--] = '0';
}

void kern_print_hex(int32_t num) {
    char buffer[11] = {0};
    int32_to_hex(num, buffer);
    kern_puts(buffer);
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

inline void kern_println(char *buffer) {
    kern_puts(buffer);
    kern_puts("\n");
}

void kernel_main() {
    terminal_initialize();
    //kern_puts("Hello from kernel mode!\nHello again :)\n");
    //kern_print_int(-123);
    
    int i = 1;
    while(i <= 25) {
        kern_print_int(i++);
        kern_puts(": Hello World!\n");
    }
    kern_puts("Thats how you do it...\n");
    kern_println("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?\n@[\\]^_`{|}~");
    kern_puts("Let's try printing to hex now: ");
    kern_print_hex((int32_t) &kernel_main);
}
