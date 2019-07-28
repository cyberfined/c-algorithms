#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "struct/bheap.h"

#define MIN_PRIORITY 0
#define MAX_PRIORITY 1000
#define NUM_TEST 2048

typedef struct {
    size_t pos;
    int    priority;
} test_node;

void test_node_gen(test_node *node) {
    node->priority = MIN_PRIORITY + rand()%(MAX_PRIORITY-MIN_PRIORITY+1);
}

int test_node_cmp(void *a, void *b) {
    return (int)a - (int)b;
}

void test_node_set_prior(bheap_node *_node, void *priority) {
    test_node *node = (test_node*)_node;
    node->priority = (int)priority;
}

void* test_node_get_prior(bheap_node *_node) {
    test_node *node = (test_node*)_node;
    return (void*)node->priority;
}

void run_test(bheap *heap) {
    assert(heap != NULL);

    // Add items to heap
    test_node node;
    for(int i = NUM_TEST; i > 0; i--) {
        test_node_gen(&node);
        assert(bheap_insert(heap, (bheap_node*)&node) != NULL);
    }
    assert(bheap_nnodes(heap) == NUM_TEST+1);

    // Pop items
    bheap_node *poped;
    void *prev_pr = (void*)MAX_PRIORITY+1, *cur_pr;
    for(int i = 0; i < NUM_TEST; i++) {
        assert((poped = bheap_pop(heap)) != NULL);
        cur_pr = test_node_get_prior(poped);
        free(poped);
        assert(test_node_cmp(prev_pr, cur_pr) >= 0);
        prev_pr = cur_pr;
    }
    assert(bheap_nnodes(heap) == 1);
}

int main() {
    bheap *heap = bheap_create(BHEAP_MIN_SIZE,
                               sizeof(test_node),
                               test_node_cmp,
                               test_node_set_prior,
                               test_node_get_prior,
                               NULL);
    srand(time(NULL));
    run_test(heap);
    bheap_free(heap);
}
