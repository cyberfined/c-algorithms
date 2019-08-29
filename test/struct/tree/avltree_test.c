#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "struct/tree/avltree.h"

#define ARR_SIZE      2048
#define NUM_RAND_TEST 10

typedef struct test_node {
    char bfactor;
    struct test_node *parent;
    struct test_node *left;
    struct test_node *right;
    int value;
} test_node;

int test_node_cmp(avlnode *a, avlnode *b) {
    return ((test_node*)a)->value - ((test_node*)b)->value;
}

size_t test_avlprops(avlnode *node) {
    if(avlnode_is_nil(node)) return 0;

    size_t l_h = test_avlprops(node->left);
    size_t r_h = test_avlprops(node->right);
    int diff = r_h - l_h;
    assert(node->bfactor == diff);

    diff = diff >= 0 ? diff : -diff;
    assert(diff < 2);

    if(!avlnode_is_nil(node->left))
        assert(test_node_cmp(node->left, node) < 0);
    if(!avlnode_is_nil(node->right))
        assert(test_node_cmp(node->right, node) > 0);

    return (l_h >= r_h ? l_h : r_h) + 1;
}

void run_arr_tests(avltree *tree, int *arr, int size, int nf_st, int nf_end, bool is_rand) {
    // Insert elements to avltree
    test_node node;
    for(int i = 0; i < size; i++) {
        node.value = arr[i];
        assert(avltree_insert(tree, (avlnode*)&node) != NULL);
        test_avlprops(tree->root);
    }
    if(!is_rand)
        assert(avltree_size(tree) == size);

    // Find elements in avltree
    test_node *found;
    for(int i = 0; i < size; i++) {
        node.value = arr[i];
        found = (test_node*)avltree_find(tree, (avlnode*)&node);
        assert(found != NULL);
        assert(found->value == arr[i]);
    }

    // Try to find nonexistent items
    for(int i = nf_st; i < nf_end; i++) {
        node.value = i;
        found = (test_node*)avltree_find(tree, (avlnode*)&node);
        assert(found == NULL);
    }

    // Delete elements from avltree
    avlnode *del;
    for(int i = 0; i < size; i++) {
        node.value = arr[i];
        del = avltree_find(tree, (avlnode*)&node);
        if(is_rand && !del) continue;
        assert(del != NULL);
        avltree_delete(tree, del);
        test_avlprops(tree->root);
    }
    assert(avltree_size(tree) == 0);
}

void run_tests(avltree *tree) {
    int arr[ARR_SIZE];

    // First test. Direct array
    for(int i = 0; i < ARR_SIZE; i++)
        arr[i] = i;
    run_arr_tests(tree, arr, ARR_SIZE, ARR_SIZE, 2*ARR_SIZE, false);

    // Second test. Reversed array
    for(int i = 0; i < ARR_SIZE; i++)
        arr[i] = ARR_SIZE-i;
    run_arr_tests(tree, arr, ARR_SIZE, ARR_SIZE+1, 2*(ARR_SIZE+1), false);

    // Third test. Random array
    for(int i = 0; i < NUM_RAND_TEST; i++) {
        for(int j = 0; j < ARR_SIZE; j++)
            arr[j] = rand() % 2048;
        run_arr_tests(tree, arr, ARR_SIZE, ARR_SIZE, 2*ARR_SIZE, true);
    }
}

int main() {
    srand(time(NULL));

    avltree *tree = avltree_create(sizeof(test_node), test_node_cmp, NULL);
    assert(tree != NULL);

    run_tests(tree);

    avltree_free(tree);
}
