#pragma once

#include <stddef.h>

#define BHEAP_MIN_SIZE 8

typedef struct {
    size_t pos;
} bheap_node;

typedef int (*bheap_comp_func)(void*,void*);
typedef void (*bheap_set_prior_func)(bheap_node*,void*);
typedef void* (*bheap_get_prior_func)(bheap_node*);
typedef void (*bheap_free_func)(bheap_node*);

typedef struct {
    size_t               heap_size;      // Max number of nodes. Must be power of 2
    size_t               num_nodes;      // Number of nodes
    size_t               node_size;      // Size of one node in bytes
    bheap_node           **nodes;        // Pointer to nodes
    bheap_comp_func      comp_func;      // Priority comparison function
    bheap_set_prior_func set_prior_func; // Priority setting function
    bheap_get_prior_func get_prior_func; // Priority getting function
    bheap_free_func      free_func;      // Node freeing function
} bheap;

bheap* bheap_create(size_t heap_size,
                    size_t node_size,
                    bheap_comp_func comp_func,
                    bheap_set_prior_func set_prior_func,
                    bheap_get_prior_func get_prior_func,
                    bheap_free_func free_func);
void bheap_heapify(bheap *heap, bheap_node *node);
void bheap_change_priority(bheap *heap, bheap_node *node, void *next_prior);
bheap_node* bheap_insert(bheap *heap, bheap_node *node);
bheap_node* bheap_pop(bheap *heap);
void bheap_free(bheap *heap);

#define bheap_nnodes(heap) (heap->num_nodes)
#define bheap_peek(heap) (heap->nodes)
