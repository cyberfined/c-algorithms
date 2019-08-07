#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "struct/tree/rbtree.h"

#define ARR_SIZE      2048
#define NUM_RAND_TEST 10

typedef struct test_node {
    char color;
    struct test_node *parent;
    struct test_node *left;
    struct test_node *right;
    int value;
} test_node;

int test_node_cmp(rbnode *a, rbnode *b) {
    return ((test_node*)a)->value - ((test_node*)b)->value;
}

size_t test_rbprops_node(rbnode *node) {
    if(rbnode_is_nil(node)) return 0;

    size_t l_h = test_rbprops_node(node->left);
    size_t r_h = test_rbprops_node(node->right);
    assert(l_h == r_h);

    if(!rbnode_is_nil(node->left))
        assert(test_node_cmp(node->left, node) < 0);
    if(!rbnode_is_nil(node->right))
        assert(test_node_cmp(node->right, node) > 0);

    if(rbnode_is_black(node))
        l_h++;
    else
        assert(rbnode_is_black(node->left) && rbnode_is_black(node->right));

    return l_h;
}

void test_rbprops_tree(rbtree *tree) {
    assert(rbnode_is_black(tree->root));
    test_rbprops_node(tree->root);
}

void run_arr_tests(rbtree *tree, int *arr, int size, int nf_st, int nf_end, bool is_rand) {
    // Insert elements to rbtree
    test_node node;
    for(int i = 0; i < size; i++) {
        node.value = arr[i];
        assert(rbtree_insert(tree, (rbnode*)&node) != NULL);
        test_rbprops_tree(tree);
    }
    if(!is_rand)
        assert(rbtree_size(tree) == size);

    // Find elements in rbtree
    test_node *found;
    for(int i = 0; i < size; i++) {
        node.value = arr[i];
        found = (test_node*)rbtree_find(tree, (rbnode*)&node);
        assert(found != NULL);
        assert(found->value == arr[i]);
    }

    // Try to find nonexistent items
    for(int i = nf_st; i < nf_end; i++) {
        node.value = i;
        found = (test_node*)rbtree_find(tree, (rbnode*)&node);
        assert(found == NULL);
    }

    // Delete elements from rbtree
    rbnode *del;
    for(int i = 0; i < size; i++) {
        node.value = arr[i];
        del = rbtree_find(tree, (rbnode*)&node);
        if(is_rand && !del) continue;
        assert(del != NULL);
        rbtree_delete(tree, del);
        test_rbprops_tree(tree);
    }
    assert(rbtree_size(tree) == 0);
}

void run_tests(rbtree *tree) {
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

    rbtree *tree = rbtree_create(sizeof(test_node), test_node_cmp, NULL);
    assert(tree != NULL);

    run_tests(tree);

    rbtree_free(tree);
}
