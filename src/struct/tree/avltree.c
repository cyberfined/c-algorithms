#include "struct/tree/avltree.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define NIL (&nil)
static avlnode nil = { .parent = NULL, .left = NIL, .right = NIL, .bfactor = 0 };

bool avlnode_is_nil(avlnode *node) {
    return node == NIL;
}

avltree* avltree_create(size_t node_size, avltree_comp_func comp_func, avltree_free_func free_func) {
    if(node_size <= sizeof(avlnode))
        return NULL;

    avltree *tree = malloc(sizeof(avltree));
    if(!tree)
        return NULL;

    *tree = (avltree) {
        .root = NIL,
        .size = 0,
        .node_size = node_size,
        .comp_func = comp_func,
        .free_func = free_func,
    };

    return tree;
}

static inline void avltree_rotate_left(avltree *tree, avlnode *node) {
    avlnode *l = node->right->left;
    node->right->parent = node->parent;
    if(node->parent == NIL)
        tree->root = node->right;
    else if(node->parent->left == node)
        node->parent->left = node->right;
    else
        node->parent->right = node->right;
    node->parent = node->right;
    node->right->left = node;
    l->parent = node;
    node->right = l;
}

static inline void avltree_rotate_right(avltree *tree, avlnode *node) {
    avlnode *r = node->left->right;
    node->left->parent = node->parent;
    if(node->parent == NIL)
        tree->root = node->left;
    else if(node->parent->left == node)
        node->parent->left = node->left;
    else
        node->parent->right = node->left;
    node->parent = node->left;
    node->left->right = node;
    r->parent = node;
    node->left = r;
}

static inline avlnode* avltree_balance_left(avltree *tree, avlnode *node) {
    avlnode *next;
    if(node->right->bfactor < 0) {
        if(node->right->left->bfactor > 0) {
            node->right->bfactor = 0;
            node->bfactor = -1;
        } else if(node->right->left->bfactor == 0) {
            node->right->bfactor = 0;
            node->bfactor = 0;
        } else {
            if(node->right != NIL)
                node->right->bfactor = 1;
            node->bfactor = 0;
        }
        node->right->left->bfactor = 0;
        next = node->right->left;
        avltree_rotate_right(tree, node->right);
    } else {
        if(node->right->bfactor == 0) {
            if(node->right != NIL)
                node->right->bfactor = -1;
            node->bfactor = 1;
        } else {
            node->bfactor = 0;
            node->right->bfactor = 0;
        }
        next = node->right;
    }
    avltree_rotate_left(tree, node);
    return next;
}

static inline avlnode* avltree_balance_right(avltree *tree, avlnode *node) {
    avlnode *next;
    if(node->left->bfactor > 0) {
        if(node->left->right->bfactor > 0) {
            if(node->left != NIL)
                node->left->bfactor = -1;
            node->bfactor = 0;
        } else if(node->left->right->bfactor == 0) {
            node->left->bfactor = 0;
            node->bfactor = 0;
        } else {
            node->left->bfactor = 0;
            node->bfactor = 1;
        }
        node->left->right->bfactor = 0;
        next = node->left->right;
        avltree_rotate_left(tree, node->left);
    } else {
        if(node->left->bfactor == 0) {
            if(node->left != NIL)
                node->left->bfactor = 1;
            node->bfactor = -1;
        } else {
            node->bfactor = 0;
            node->left->bfactor = 0;
        }
        next = node->left;
    }
    avltree_rotate_right(tree, node);
    return next;
}

static inline void avltree_insert_fixup(avltree *tree, avlnode *node) {
    for(;;) {
        if(node == tree->root)
            break;

        if(node->parent->left == node)
            node->parent->bfactor--;
        else
            node->parent->bfactor++;
        
        if(node->parent->bfactor == 2)
            node = avltree_balance_left(tree, node->parent);
        else if(node->parent->bfactor == -2)
            node = avltree_balance_right(tree, node->parent);
        else
            node = node->parent;

        if(node->bfactor == 0)
            break;
    }
}

avlnode* avltree_insert(avltree *tree, avlnode *node) {
    avlnode *i, *p;
    avlnode **place;
    avlnode *newnode;
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
            memcpy(i+1, node+1, tree->node_size - sizeof(avlnode));
            return i;
        }
    }

    newnode = malloc(tree->node_size);
    if(!newnode)
        return NULL;
    memcpy(newnode+1, node+1, tree->node_size - sizeof(avlnode));
    newnode->bfactor = 0;
    newnode->parent = p;
    newnode->left = NIL;
    newnode->right = NIL;
    *place = newnode;
    tree->size++;
    avltree_insert_fixup(tree, newnode);
    return newnode;
}

avlnode* avltree_find(avltree *tree, avlnode *key) {
    avlnode *i = tree->root;
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

static inline void avltree_delete_fixup(avltree *tree, avlnode *node) {
    for(;;) {
        if(node->bfactor == 2)
            node = avltree_balance_left(tree, node);
        else if(node->bfactor == -2)
            node = avltree_balance_right(tree, node);
        else if(node == tree->root || node->bfactor != 0)
            break;
        else {
            if(node->parent->left == node)
                node->parent->bfactor++;
            else
                node->parent->bfactor--;
            node = node->parent;
        }
    }
}

void avltree_delete(avltree *tree, avlnode *node) {
    avlnode *x, *y;

    if(node->right == NIL) {
        x = node->left;
    } else {
        x = node->right;
        while(x->left != NIL)
            x = x->left;

        if(x->parent != node) {
            y = x->parent;
            x->parent->left = x->right;
            x->right->parent = x->parent;
            x->right = node->right;
            x->right->parent = x;
        } 

        x->left = node->left;
        x->left->parent = x;
    }

    if(x != NIL) {
        if(x->parent == node)
            y = x;
        if(x->parent->right == x)
            x->parent->bfactor--;
        else
            x->parent->bfactor++;
        x->bfactor = node->bfactor;
        x->parent = node->parent;
    } else {
        y = node->parent;
        if(y->left == node)
            y->bfactor++;
        else
            y->bfactor--;
    }

    if(node == tree->root)
        tree->root = x;
    else if(node->parent->left == node)
        node->parent->left = x;
    else
        node->parent->right = x;

    avltree_delete_fixup(tree, y);

    tree->size--;

    if(tree->free_func)
        tree->free_func(node);
    free(node);
}

static void avlnode_free(avltree *tree, avlnode *node) {
    avlnode *next;
    while(node != NIL) {
        if(node->bfactor <= 0) {
            avlnode_free(tree, node->right);
            next = node->left;
        } else {
            avlnode_free(tree, node->left);
            next = node->right;
        }
        if(tree->free_func)
            tree->free_func(node);
        free(node);
        node = next;
    }
}

void avltree_free(avltree *tree) {
    avlnode_free(tree, tree->root);
    free(tree);
}
