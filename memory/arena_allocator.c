#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

int arena_allocator_create(arena_allocator_t* a, size_t cap) {
    if (!a)
	return -1;
    a->pool = malloc(cap);
    if (!a->pool)
	return -2;
    a->capacity = cap;
    a->current_offset = 0;
    a->backed = 0;
    return 0;
}

int arena_allocator_create_backed(arena_allocator_t* a, unsigned char* buffer, size_t cap) {
    if (!a || !buffer)
	return -1;
    a->pool = buffer;
    a->capacity = cap;
    a->current_offset = 0;
    a->backed = 1;
    return 0;    
}

void* arena_allocator_alloc(arena_allocator_t* a, size_t size) {
    return arena_allocator_alloc_align(a, size, 4);
}

void* arena_allocator_alloc_align(arena_allocator_t* a, size_t size, size_t alignment) {
    size_t padding;

    if (!a || !a->pool)
	return NULL;

    if (!power_of_two(alignment))
	return NULL;

    padding = (size_t)(a->pool + a->current_offset) % alignment;

    if (padding > 0) {
	padding = alignment - padding;
    }

    if (a->current_offset + padding + size >= a->capacity)
	return NULL;
    a->current_offset += padding + size;
    return a->pool + a->current_offset - size;
}

void arena_allocator_destroy(arena_allocator_t* a) {
    if (!a || !a->pool)
	return;
    if (!a->backed)
	free(a->pool);
    a->pool = NULL;
    a->capacity = a->current_offset = 0;
}
void arena_allocator_reset(arena_allocator_t* a) {
    if (!a)
	return;
    a->current_offset = 0;
}

void arena_allocator_print(arena_allocator_t* a) {
    size_t i;
    printf("cap: %d, curr_off: %d\n", a->capacity, a->current_offset);
    for (i = 0; i < a->current_offset; i++)
	printf("%02x", *(a->pool + i));
}

