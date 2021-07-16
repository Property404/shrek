#include "io.h"
#include "cmisc.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define ARRAY_LENGTH(x) sizeof(x)/sizeof(*x)

void puts(const char* buffer) {
    if(buffer == NULL) {
        puts("(NULL)");
        return;
    }
    for(size_t i=0; buffer[i] != '\0';i++) {
        putchar(buffer[i]);
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

static void puthex(uint32_t val) {
    const char* hex_digits = "0123456789ABCDEF";
    for(signed i=sizeof(val)-1;i>=0;i--) {
        uint8_t byte = (val >> (8*i)) & 0xFF;
        putchar(hex_digits[byte/16]);
        putchar(hex_digits[byte%16]);
    }
}

typedef struct {
    const char* flag;
    unsigned width;
    const char* length;
    const char* type;

    const char* end;
}FormatPlaceholder ;

/* Check if `string` begins with any of the prefixes contained in `set`
 * If yes, return a pointer to the prefix in `set`
 * If no, return NULL
 *
 * `Set` is a null-delimited list. We do it this stupid way so we can use `printf`
 * very early, before paging has been set up.
 *
 * If we were to pass an array of char*'s, each char* would point to the VIRTUAL
 * address, because those aren't affected by the GOT, even though the pointer
 * TO the array would be
 *
 * Sets end with two nulls
 * */
static const char* get_first_match(
    const char* __restrict__ string,
    const char* __restrict__ set
) {
    while(*set != '\0') {
        const size_t length = strlen(set);
        if (strncmp(set, string, length) == 0) {
            return set;
        }
        set += length + 1;
    }
    return NULL;
}

static int parse_placeholder(const char* fmt, FormatPlaceholder* placeholder) {
    /*https://en.wikipedia.org/wiki/Printf_format_string#Format_placeholder_specification*/

    // These HAVE to end with an extra '\0'
    const char* FORMAT_FLAGS = "0\0";
    //const char* FORMAT_LENGTHS = "ll\0l\0q\0";
    const char* FORMAT_TYPES = "p\0x\0s\0c\0d\0i\0";

    placeholder->flag = NULL;
    placeholder->width = 0;
    placeholder->length = NULL;
    placeholder->type = NULL;

    const char* ptr = fmt;

    // Find flags
    placeholder->flag = get_first_match(ptr, FORMAT_FLAGS);
    if(placeholder->flag)ptr+=strlen(placeholder->flag);

    // Find width
    while(string_has("0123456789", *ptr)) {
        placeholder->width*=10;
        placeholder->width+=*ptr - '0';
        ptr++;
    }

    // Find type
    placeholder->type = get_first_match(ptr, FORMAT_TYPES);
    if(placeholder->type)ptr+=strlen(placeholder->type);

    placeholder->end = ptr;

    return 0;
}


void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while(*fmt != 0) {
        if (*fmt == '%') {
            fmt++;
            FormatPlaceholder placeholder;
            if(parse_placeholder(fmt, &placeholder)) {
                puts("Failed to parse placeholder!\n");
                return;
            }
            fmt = placeholder.end;
            switch (*(placeholder.type)) {
                case 'x':
                    puthex(va_arg(args, unsigned));
                    break;
                case 'p':
                    puts("0x");
                    puthex(va_arg(args, unsigned));
                    break;
                case 's':
                    puts(va_arg(args, char*));
                    break;
                case 'c':
                    // integer promotion happens here
                    putchar((va_arg(args, int)));
                    break;
                case '%':
                    putchar('%');
                    break;
                default:
                    puts("[INVALID_TYPE:");
                    puts(placeholder.type);
                    puts("]");
            }
            continue;
        } else {
            putchar(*fmt);
        }
        fmt++;
    }
    va_end(args);

}
