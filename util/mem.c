#include <stddef.h> 
void kern_memset0(void *buffer, size_t n);

void kern_memcpy(void *dest, const void *src, size_t n) {
    while(n--)
        *(char *) dest++ = *(char *) src++;
}

void kern_memset(void *buffer, char byte, size_t n) {
    if(byte == 0) {
        kern_memset0(buffer, n);
        return;
    }

    while(n--)
        *(char *) buffer++ = byte;
}

void kern_memset0(void *buffer, size_t n) {
    while(n--)
        *(char *) buffer++ = 0;
}
