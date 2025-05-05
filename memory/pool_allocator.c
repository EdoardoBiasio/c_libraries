#include "memory.h"
#include <stdlib.h>
#include <string.h>

int pool_allocator_create(pool_allocator_t* p, size_t cap, size_t block_size, size_t block_alignment) {
    size_t rem;

    if (!p)
	return -1;
    p->capacity = cap;
    rem = block_size % block_alignment;
    if (rem > 0)
	rem = block_alignment - rem;
    p->block_size = block_size + rem;
    p->pool = calloc(p->block_size, p->capacity);
    p->backed = 0;
    pool_allocator_free_all(p);
    return 0;
}

int pool_allocator_create_backed(pool_allocator_t* p, unsigned char* buffer, size_t buffer_size, size_t cap, size_t block_size, size_t block_alignment) {
    size_t rem;

    if (!p)
	return -1;
    p->capacity = cap;
    rem = block_size % block_alignment;
    
    if (rem > 0)
	rem = block_alignment - rem;
    p->block_size = block_size + rem;
    if (p->block_size * p->capacity > buffer_size)
	return -1;
    
    p->pool = buffer;
    p->backed = 1;
    pool_allocator_free_all(p);
    return 0;
}

void pool_allocator_destroy(pool_allocator_t* p) {
    if (!p)
	return;

    if (!p->backed)
	free(p->pool);

    p->capacity = p->block_size = p->backed = 0;
    p->fl_head = NULL;
}

void* pool_allocator_alloc(pool_allocator_t* p) {
    pool_allocator_block_t* ptr;

    if (!p)
	return NULL;
    
    ptr = p->fl_head;
    p->fl_head = ptr->next;
    memset(ptr, 0, p->block_size);
    
    return ptr;
}

void pool_allocator_free(pool_allocator_t* p, void* ptr) {
    pool_allocator_block_t* block;

    if (!p || !ptr)
	return;

    block = (pool_allocator_block_t*) ptr;
    block->next = p->fl_head;
    p->fl_head = block;
}

void pool_allocator_free_all(pool_allocator_t* p) {
    int i;
    pool_allocator_block_t* ptr;
    if (!ptr)
	return;
    p->fl_head = NULL;
    for (i = 0; i < p->capacity; i++) {
	ptr = (pool_allocator_block_t*) &p->pool[p->block_size * i];
	ptr->next = p->fl_head;
	p->fl_head = ptr;
    }
}

