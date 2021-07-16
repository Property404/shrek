#include "cmisc.h"
#include <stdbool.h>

void memset(void* dest, uint8_t byte, size_t length) {
    uint8_t* destination = dest;
    for(size_t i=0;i<length;i++) {
        destination[i] = byte;
    }
}

void memcpy(void*restrict dest, const void*restrict src, size_t length) {
    for(size_t i=0;i<length;i++) {
        ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
    }
}

size_t strlen(const char* string) {
    size_t i;
    for(i=0; string[i] != '\0';i++);
    return i;
}

int strncmp(const char* s1, const char* s2, size_t length) {
    if(length == 0)
        return 0;
    if(s1 == s2)
        return 0;

    for(size_t i=0;i<length;i++) {
        if (s1[i] > s2[i])
            return 1;
        if (s1[i] < s2[i])
            return -1;
        if(s1[i] == '\0' && s2[i] == '\0')
            return 0;
        if(s1[i] == '\0')
            return -1;
        if(s2[i] == '\0')
            return 1;
    }

    return 0;
}

int strcmp(const char* s1, const char* s2) {
    return strncmp(s1, s2, SIZE_MAX);
}

bool string_has(const char* string, char character) {
    for(size_t i=0; string[i] != '\0'; i++) {
        if(string[i] == character)
            return true;
    }
    return false;
}

uint32_t string_to_u32(const char* str) {
    // Get rid of 0x before hex
    while(string_has("0xX", *str))str++;

    uint32_t value = 0;
    for(size_t i=0; string_has("0123456789ABCDEFabcdef", str[i]);i++) {
        value *= 16;
        if(string_has("0123456789", str[i]))
            value += str[i] - '0';
        else if(string_has("abcdef", str[i]))
            value += str[i] - 'a' + 0xa;
        else
            value += str[i] - 'A' + 0xa;
    } 

    return value;
}

void* string_to_pointer(const char* str) {
    _Static_assert(sizeof (void*) == sizeof (uintptr_t),
            "Can't determine pointer size");
#if UINTPTR_MAX == 0xFFFFFFFF
    return (void*)(string_to_u32(str));
#else
#error Not implemented for this pointer size
#endif
}


char* strsep(char** restrict stringp, const char* restrict delim)
{
    char* ptr = *stringp;
    if(ptr == NULL || *ptr == '\0')
        return NULL;

    // Don't worry about being in a delim
    while(string_has(delim, *ptr)) {
        ptr++;
        if (*ptr == '\0')
            return NULL;
    }
    *stringp = ptr;


    while(true) {
        if(string_has(delim, *ptr)) {
            char* rval = *stringp;
            *ptr = '\0';
            *stringp = ptr + 1;
            return rval;
        }

        ptr ++;
        if(*ptr == '\0') {
            char* rval = *stringp;
            *stringp = NULL;
            return rval;
        }
    }
}

