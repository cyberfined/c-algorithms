#include "struct/tree/rbtree.h"

#include <stdlib.h>
#include <string.h>

#define NIL (&nil)
static rbnode nil = { .parent = NULL, .left = NIL, .right = NIL, .color = RBBLACK };

bool rbnode_is_nil(rbnode *node) {
    return node == NIL;
}

rbtree* rbtree_create(size_t node_size, rbtree_comp_func comp_func, rbtree_free_func free_func) {
    if(node_size <= sizeof(rbnode))
        return NULL;

    rbtree *tree = malloc(sizeof(rbtree));
    if(!tree)
        return NULL;

    tree->root = NIL;
    tree->size = 0;
    tree->node_size = node_size;
    tree->comp_func = comp_func;
    tree->free_func = free_func;
    return tree;
}

static void rbtree_rotate_left(rbtree *tree, rbnode *node) {
    rbnode *l = node->right->left;

    node->right->parent = node->parent;
    if(node->parent == NIL)
        tree->root = node->right;
    else if(node->parent->left == node)
        node->parent->left = node->right;
    else
        node->parent->right = node->right;
    node->parent = node->right;
    node->right->left = node;

    node->right = l;
    if(l != NIL)
        l->parent = node;
}

static void rbtree_rotate_right(rbtree *tree, rbnode *node) {
    rbnode *r = node->left->right;

    node->left->parent = node->parent;
    if(node->parent == NIL)
        tree->root = node->left;
    else if(node->parent->left == node)
        node->parent->left = node->left;
    else
        node->parent->right = node->left;
    node->parent = node->left;
    node->left->right = node;
    
    node->left = r;
    if(r != NIL)
        r->parent = node;
}

static inline void rbtree_insert_fixup(rbtree *tree, rbnode *node) {
    rbnode *x;
    while(node->parent->color == RBRED) {
        if(node->parent->parent->left == node->parent) {
            x = node->parent->parent->right;
            if(x->color == RBRED) {
                node->parent->color = RBBLACK;
                x->color = RBBLACK;
                node->parent->parent->color = RBRED;
                node = node->parent->parent;
            } else {
                if(node->parent->right == node) {
                    node = node->parent;
                    rbtree_rotate_left(tree, node);
                }
                node->parent->parent->color = RBRED;
                node->parent->color = RBBLACK;
                rbtree_rotate_right(tree, node->parent->parent);
            }
        } else {
            x = node->parent->parent->left;
            if(x->color == RBRED) {
                node->parent->color = RBBLACK;
                x->color = RBBLACK;
                node->parent->parent->color = RBRED;
                node = node->parent->parent;
            } else {
                if(node->parent->left == node) {
                    node = node->parent;
                    rbtree_rotate_right(tree, node);
                }
                node->parent->parent->color = RBRED;
                node->parent->color = RBBLACK;
                rbtree_rotate_left(tree, node->parent->parent);
            }
        }
    }
    tree->root->color = RBBLACK;
}

rbnode* rbtree_insert(rbtree *tree, rbnode *node) {
    rbnode *i, *p;
    rbnode **place;
    rbnode *newnode;
    int c;

    i = tree->root;
    place = &tree->root;
    p = NIL;

    while(i != NIL) {
        p = i;
        c = tree->comp_func(node, i);
        if(c < 0) {
            place = &i->left;
            i = i->left;
        } else if(c > 0) {
            place = &i->right;
            i = i->right;
        } else {
            if(tree->free_func)
                tree->free_func(i);
            memcpy(i+1, node+1, tree->node_size - sizeof(rbnode));
            return i;
        }
    }

    newnode = malloc(tree->node_size);
    if(!newnode)
        return NULL;
    memcpy(newnode+1, node+1, tree->node_size - sizeof(rbnode));
    newnode->color = RBRED;
    newnode->parent = p;
    newnode->left = NIL;
    newnode->right = NIL;
    *place = newnode;
    tree->size++;
    rbtree_insert_fixup(tree, newnode);
    return newnode;
}

rbnode* rbtree_find(rbtree *tree, rbnode *key) {
    rbnode *i = tree->root;
    int c;

    while(i != NIL) {
        c = tree->comp_func(key, i);
        if(c == 0)
            return i;
        else if(c < 0)
            i = i->left;
        else
            i = i->right;
    }

    return NULL;
}

static inline void rbtree_transplant(rbtree *tree, rbnode *x, rbnode *y) {
    if(x->parent == NIL)
        tree->root = y;
    else if(x->parent->left == x)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->parent = x->parent;
}

static inline void rbtree_delete_fixup(rbtree *tree, rbnode *node) {
    rbnode *x;

    while(node != tree->root && node->color == RBBLACK) {
        if(node->parent->left == node) {
            x = node->parent->right;
            if(x->color == RBRED) {
                x->color = RBBLACK;
                x->parent->color = RBRED;
                rbtree_rotate_left(tree, x->parent);
                x = node->parent->right;
            }

            if(x->left->color == RBBLACK && x->right->color == RBBLACK) {
                x->color = RBRED;
                node = node->parent;
            } else {
                if(x->right->color == RBBLACK) {
                    x->left->color = RBBLACK;
                    x->color = RBRED;
                    rbtree_rotate_right(tree, x);
                    x = x->parent;
                }

                x->color = node->parent->color;
                node->parent->color = RBBLACK;
                x->right->color = RBBLACK;
                rbtree_rotate_left(tree, x->parent);
                break;
            }
        } else {
            x = node->parent->left;
            if(x->color == RBRED) {
                x->color = RBBLACK;
                x->parent->color = RBRED;
                rbtree_rotate_right(tree, x->parent);
                x = node->parent->left;
            }

            if(x->left->color == RBBLACK && x->right->color == RBBLACK) {
                x->color = RBRED;
                node = node->parent;
            } else {
                if(x->left->color == RBBLACK) {
                    x->right->color = RBBLACK;
                    x->color = RBRED;
                    rbtree_rotate_left(tree, x);
                    x = x->parent;
                }

               x->color = node->parent->color;
               node->parent->color = RBBLACK;
               x->left->color = RBBLACK; 
               rbtree_rotate_right(tree, node->parent);
               break;
            }
        }
    }

    node->color = RBBLACK;
}

void rbtree_delete(rbtree *tree, rbnode *node) {
    char color;
    rbnode *x, *y;

    if(!node || node == NIL)
        return;

    color = node->color;
    if(node->left == NIL) {
        x = node->right;
        rbtree_transplant(tree, node, node->right);
    } else if(node->right == NIL) {
        x = node->left;
        rbtree_transplant(tree, node, node->left);
    } else {
        x = node->right;
        while(x->left != NIL)
            x = x->left;
        color = x->color;
        y = x->right;

        if(x->parent == node) {
            y->parent = x;
        } else {
            rbtree_transplant(tree, x, x->right);
            x->right = node->right;
            x->right->parent = x;
        }

        rbtree_transplant(tree, node, x);
        x->left = node->left;
        x->left->parent = x;
        x->color = node->color;

        x = y;
    }

    tree->size--;

    if(color == RBBLACK)
        rbtree_delete_fixup(tree, x);

    if(tree->free_func)
        tree->free_func(node);
    free(node);
}

static void rbnode_free(rbtree *tree, rbnode *node) {
    rbnode *next;
    while(node != NIL) {
        rbnode_free(tree, node->right);
        next = node->left;
        if(tree->free_func)
            tree->free_func(node);
        free(node);
        node = next;
    }
}

void rbtree_free(rbtree *tree) {
    rbnode_free(tree, tree->root);
    free(tree);
}
