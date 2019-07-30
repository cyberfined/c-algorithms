#pragma once

#include <stddef.h>

#define SWAPBRANCH(type, a, b, n) { \
        size_t i = (n)/sizeof(type); \
        type tmp; \
        type *pa = (type*)(a); \
        type *pb = (type*)(b); \
        do { \
            tmp = *pa; \
            *pa++ = *pb; \
            *pb++ = tmp; \
        } while(--i); \
    }

static inline void swap(const void *a, const void *b, size_t n) {
    if(n % sizeof(long) == 0)
        SWAPBRANCH(long, a, b, n)
    else
        SWAPBRANCH(char, a, b, n)
}

typedef int (*comp_func)(const void*, const void*);
