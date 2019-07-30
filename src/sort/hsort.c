#include "sort/hsort.h"
#include "sort/common.h"

static inline void heapify(void *base, size_t nmemb, size_t size, comp_func cmp, size_t i) {
    size_t l, r, largest;

    for(;;) {
        l = 2*i + 1;
        r = 2*i + 2;
        if(l < nmemb && cmp(base + l*size, base + i*size) > 0)
            largest = l;
        else largest = i;
        if(r < nmemb && cmp(base + r*size, base + largest*size) > 0)
            largest = r;

        if(largest == i)
            return;

        swap(base + i*size, base + largest*size, size);
        i = largest;
    }
}

void heapsort(void *base, size_t nmemb, size_t size, comp_func cmp) {
    for(int i = (nmemb-2)/2; i >= 0; i--)
        heapify(base, nmemb, size, cmp, i);
    
    for(size_t i = nmemb-1; i >= 1; i--) {
        swap(base, base + i*size, size);
        nmemb--;
        heapify(base, nmemb, size, cmp, 0);
    }
}
