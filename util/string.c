#include <stdint.h>

uint32_t kern_strlen(const char *str) {
    const char *start = str;
    for( ; *start != '\x00' ; start++);
    return start - str;
}

void kern_strcpy(char *dest, const char *src) {
    uint32_t src_size = kern_strlen(src);
    for(uint32_t i = 0 ; i < src_size ; i++)
        *(dest + i) = *(src + i);
}
