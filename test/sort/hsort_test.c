#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "sort/hsort.h"

#define ARRAY_SIZE 128
#define MIN_ELEMENT 0
#define MAX_ELEMENT 5000

int int_cmp(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

void test_array(const void *arr, size_t size) {
    for(size_t i = 0; i < size-1; i++)
        assert(int_cmp(arr + i*sizeof(int), arr + (i+1)*sizeof(int)) <= 0);
}

int main() {
    int arr[ARRAY_SIZE];
    srand(time(NULL));
    
    // Best case
    for(int i = 0; i < ARRAY_SIZE; i++)
        arr[i] = i;
    heapsort(arr, ARRAY_SIZE, sizeof(int), int_cmp);
    test_array(arr, ARRAY_SIZE);

    // Worst case
    for(int i = 0; i < ARRAY_SIZE; i++)
        arr[i] = ARRAY_SIZE-i;
    heapsort(arr, ARRAY_SIZE, sizeof(int), int_cmp);
    test_array(arr, ARRAY_SIZE);

    // Random array
    for(int i = 0; i < ARRAY_SIZE; i++)
        arr[i] = MIN_ELEMENT + rand()%(MAX_ELEMENT-MIN_ELEMENT+1);
    heapsort(arr, ARRAY_SIZE, sizeof(int), int_cmp);
    test_array(arr, ARRAY_SIZE);
}
