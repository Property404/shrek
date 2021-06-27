#include "io.h"
#include <stddef.h>
#include <stdint.h>

void puts(const char* buffer) {
    for(size_t i=0; buffer[i] != '\0';i++) {
        putchar(buffer[i]);
    }
}
void puthex(uint32_t val) {
    const char* hex_digits = "0123456789ABCDEF";
    puts("0x");
    for(signed i=sizeof(val)-1;i>=0;i--) {
        uint8_t byte = (val >> (8*i)) & 0xFF;
        putchar(hex_digits[byte/16]);
        putchar(hex_digits[byte%16]);
    }
}


void nputs(const char* buffer, size_t len) {
    for(size_t i=0; i<len;i++) {
        char c = buffer[i];
        if (c == '\0') {
            c = '.';
        } else if ( c < 0x20 || c > 0x7f) {
            c = '?';
        }
        putchar(c);
    }
}

