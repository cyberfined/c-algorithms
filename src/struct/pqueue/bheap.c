#include "struct/pqueue/bheap.h"

#include <stdlib.h>
#include <string.h>

#define parent(i) (i>>1)
#define left(i) (i<<1)
#define right(i) ((i<<1)+1)

static inline __attribute__((always_inline))
size_t next_power_of_two(size_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

bheap* bheap_create(size_t heap_size,
                    size_t node_size,
                    bheap_comp_func comp_func,
                    bheap_set_prior_func set_prior_func,
                    bheap_get_prior_func get_prior_func,
                    bheap_free_func free_func)
{
    bheap *heap = malloc(sizeof(bheap));
    if(!heap)
        return NULL;

    if(heap_size < BHEAP_MIN_SIZE) heap_size = BHEAP_MIN_SIZE;
    else if((heap_size & (heap_size-1))) heap_size = next_power_of_two(heap_size);

    heap->nodes = malloc(sizeof(bheap_node*) * (heap_size+1));
    if(!heap->nodes) {
        free(heap);
        return NULL;
    }

    heap->num_nodes = 1;
    heap->heap_size = heap_size;
    heap->node_size = node_size;
    heap->comp_func = comp_func;
    heap->set_prior_func = set_prior_func;
    heap->get_prior_func = get_prior_func;
    heap->free_func = free_func;

    return heap;
}

void bheap_heapify(bheap *heap, bheap_node *node) {
    if(node->pos == 0 || node->pos >= heap->num_nodes)
        return;

    size_t i, l, r, largest, tmp_pos;
    void *pr_a, *pr_b;
    bheap_node *tmp_node;

    i = node->pos;
    while(i < heap->num_nodes) {
        l = left(i);
        r = right(i);

        largest = i;
        if(l < heap->num_nodes) {
            pr_a = heap->get_prior_func(heap->nodes[l]);
            pr_b = heap->get_prior_func(heap->nodes[i]);
            if(heap->comp_func(pr_a, pr_b) > 0) largest = l;
        }

        if(r < heap->num_nodes) {
            pr_a = heap->get_prior_func(heap->nodes[r]);
            pr_b = heap->get_prior_func(heap->nodes[largest]);
            if(heap->comp_func(pr_a, pr_b) > 0) largest = r;
        }

        if(largest == i)
            break;
        
        tmp_node = heap->nodes[i];
        tmp_pos = heap->nodes[largest]->pos;

        heap->nodes[i] = heap->nodes[largest];
        heap->nodes[i]->pos = tmp_node->pos;

        heap->nodes[largest] = tmp_node;
        heap->nodes[largest]->pos = tmp_pos;

        i = largest;
    }
}

static void bheap_moveup(bheap *heap, bheap_node *node) {
    size_t i, p;
    bheap_node *tmp_node;
    void *par_pr, *prior;

    prior = heap->get_prior_func(node);
    i = node->pos;
    while(i > 1) {
        p = parent(i);
        par_pr = heap->get_prior_func(heap->nodes[p]);
        if(heap->comp_func(par_pr, prior) >= 0)
            break;

        tmp_node = heap->nodes[i];

        heap->nodes[i] = heap->nodes[p];
        heap->nodes[i]->pos = i;

        heap->nodes[p] = tmp_node;
        heap->nodes[p]->pos = p;

        i = p;
    }
}

void bheap_change_priority(bheap *heap, bheap_node *node, void *next_prior) {
    void *cur_prior = heap->get_prior_func(node);
    if(heap->comp_func(cur_prior, next_prior) >= 0)
        return;

    heap->set_prior_func(node, next_prior);
    bheap_moveup(heap, node);
}

bheap_node* bheap_insert(bheap *heap, bheap_node *node) {
    if(heap->num_nodes > heap->heap_size) {
        heap->heap_size <<= 1;
        heap->nodes = realloc(heap->nodes, sizeof(bheap_node*) * (heap->heap_size+1));
        if(!heap->nodes)
            return NULL;
    }

    bheap_node *new_node = malloc(heap->node_size);
    if(!new_node)
        return NULL;
    memcpy(new_node, node, heap->node_size);
    new_node->pos = heap->num_nodes;
    heap->nodes[heap->num_nodes++] = new_node;
    bheap_moveup(heap, new_node);
    return new_node;
}

bheap_node* bheap_pop(bheap *heap) {
    if(heap->num_nodes == 1)
        return NULL;

    bheap_node *max = heap->nodes[1];
    heap->nodes[1] = heap->nodes[--heap->num_nodes]; 
    heap->nodes[1]->pos = 1;

    if(heap->heap_size > BHEAP_MIN_SIZE && (heap->num_nodes<<1) == (heap->heap_size+2)) {
        heap->heap_size >>= 1;
        heap->nodes = realloc(heap->nodes, sizeof(bheap_node*) * (heap->heap_size+1));
        if(!heap->nodes)
            return NULL;
    }

    bheap_heapify(heap, heap->nodes[1]);

    return max;
}

void bheap_free(bheap *heap) {
    free(heap->nodes[0]);
    for(int i = 1; i < heap->num_nodes; i++) {
        if(heap->free_func)
            heap->free_func(heap->nodes[i]);
        free(heap->nodes[i]);
    }
    free(heap->nodes);
    free(heap);
}
