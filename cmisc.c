#include "cmisc.h"

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
