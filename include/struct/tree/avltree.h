#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct avlnode {
    char bfactor;
    struct avlnode *parent;
    struct avlnode *left;
    struct avlnode *right;
} avlnode;

bool avlnode_is_nil(avlnode *node);

typedef int (*avltree_comp_func)(avlnode*,avlnode*);
typedef void (*avltree_free_func)(avlnode*);

typedef struct {
    avlnode           *root;
    size_t            size;
    size_t            node_size;
    avltree_comp_func comp_func;
    avltree_free_func free_func;
} avltree;

avltree* avltree_create(size_t node_size, avltree_comp_func comp_func, avltree_free_func free_func);
avlnode* avltree_insert(avltree *tree, avlnode *node);
avlnode* avltree_find(avltree *tree, avlnode *key);
void avltree_delete(avltree *tree, avlnode *node);
void avltree_free(avltree *tree);

#define avltree_size(tree) (tree->size)
