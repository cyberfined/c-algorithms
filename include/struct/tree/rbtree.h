#pragma once

#include <stdbool.h>
#include <stddef.h>

#define RBRED (0)
#define RBBLACK (1)

typedef struct rbnode {
    char color;
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
} rbnode;

#define rbnode_is_red(node) (node->color == RBRED)
#define rbnode_is_black(node) (node->color == RBBLACK)
bool rbnode_is_nil(rbnode *node);

typedef int (*rbtree_comp_func)(rbnode*,rbnode*);
typedef void (*rbtree_free_func)(rbnode*);

typedef struct {
    rbnode           *root;
    size_t           size;
    size_t           node_size;
    rbtree_comp_func comp_func;
    rbtree_free_func free_func;
} rbtree;

rbtree* rbtree_create(size_t node_size, rbtree_comp_func comp_func, rbtree_free_func free_func);
rbnode* rbtree_insert(rbtree *tree, rbnode *node);
rbnode* rbtree_find(rbtree *tree, rbnode *key);
void rbtree_delete(rbtree *tree, rbnode *node);
void rbtree_free(rbtree *tree);

#define rbtree_size(tree) (tree->size)
