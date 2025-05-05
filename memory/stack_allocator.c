#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct small_header_s {
    unsigned short padding;
} small_header_t;

int small_stack_allocator_create(small_stack_allocator_t* s, size_t cap) {
    unsigned char* pool;
    if (!s)
	return -1;

    pool = malloc(cap);
    if (!pool)
	return -2;
    s->pool = pool;
    s->capacity = cap;
    s->offset = 0;
    return 0;
}

void small_stack_allocator_free(small_stack_allocator_t* s, void* ptr) {
    small_header_t *h;
    if (ptr == NULL || s == NULL)
	return;

    if ((unsigned char*)ptr < s->pool || (unsigned char*)ptr > s->pool + s->capacity || (unsigned char*)ptr > s->pool + s->offset)
	return;

    h = ptr - sizeof(small_header_t);
    s->offset = ((unsigned char*)ptr - s->pool);
    s->offset -= h->padding;
}

int power_of_two(size_t value) {
    
    return value & (value - 1) == 0;
}


void* small_stack_allocator_alloc_align(small_stack_allocator_t* s, size_t size, size_t alignment) {
    unsigned char* curr;
    size_t pad;
    small_header_t* h;
    size_t pad_total;
    if (!s)
	return NULL;
    if (!power_of_two(alignment))
	return NULL;

    curr = s->pool + s->offset;

    pad = (size_t)curr % alignment;
    if (pad > 0)
        pad = alignment - pad;
    
    if (pad < sizeof(small_header_t)) {
	pad_total = sizeof(small_header_t) - pad;
	if (pad_total % alignment == 0)
	    pad_total = pad + alignment * (pad_total / alignment);
	else
	    pad_total = pad + alignment * (pad_total / alignment + 1);
    }
    
    if (s->offset + size >= s->capacity)
	return NULL;

    s->offset += pad_total;
    
    h = (small_header_t*) (s->pool + s->offset - sizeof(small_header_t));
    h->padding = pad_total;
    memset(s->pool + s->offset, 0, size);
    s->offset += size;

    return s->pool + s->offset - size;
}

void* small_stack_allocator_alloc(small_stack_allocator_t* s, size_t size) {
    return small_stack_allocator_alloc_align(s, size, 4);
}

void small_stack_allocator_destroy(small_stack_allocator_t* s) {
    if (!s)
	return;
    if (!s->pool)
	return;
    free(s->pool);
    s->pool = NULL;
    s->capacity = s->offset = 0;
}

void small_small_stack_allocator_print(small_stack_allocator_t* s) {
    size_t i;
    printf("cap: %ul, len: %ul\n", s->capacity, s->offset);
    for (i = 0; i < s->offset; i++)
	printf("%02x", *(s->pool + i));
    printf("\n");
    for (i = 0; i < s->offset; i++)
	printf("%u ", *(s->pool + i));
    printf("\n");
}


typedef struct header_s {
    size_t prev_offset;
    size_t padding;
} header_t;

int stack_allocator_create(stack_allocator_t* s, size_t cap) {
    unsigned char* pool;
    if (!s)
	return -1;

    pool = malloc(cap);
    if (!pool)
	return -2;
    s->pool = pool;
    s->capacity = cap;
    s->offset = 0;
    s->prev_offset = 0;
    return 0;
}

void stack_allocator_free(stack_allocator_t* s, void* ptr) {
    header_t *h;
    if (ptr == NULL || s == NULL)
	return;

    if ((unsigned char*)ptr < s->pool || (unsigned char*)ptr > s->pool + s->capacity || (unsigned char*)ptr > s->pool + s->offset)
	return;

    h = ptr - sizeof(header_t);
    if (h->prev_offset != s->prev_offset)
	return;
    s->offset = ((unsigned char*)ptr - s->pool);
    s->offset -= h->padding;
}


void* stack_allocator_alloc_align(stack_allocator_t* s, size_t size, size_t alignment) {
    unsigned char* curr;
    size_t pad;
    header_t* h;
    size_t pad_total;
    if (!s)
	return NULL;
    if (!power_of_two(alignment))
	return NULL;

    curr = s->pool + s->offset;

    pad = (size_t)curr % alignment;
    if (pad > 0)
        pad = alignment - pad;
    
    if (pad < sizeof(header_t)) {
	pad_total = sizeof(header_t) - pad;
	if (pad_total % alignment == 0)
	    pad_total = pad + alignment * (pad_total / alignment);
	else
	    pad_total = pad + alignment * (pad_total / alignment + 1);
    }
    
    if (s->offset + size >= s->capacity)
	return NULL;

    s->prev_offset = s->offset;
    s->offset += pad_total;
    
    h = (header_t*) (s->pool + s->offset - sizeof(header_t));
    h->padding = pad_total;
    h->prev_offset = s->prev_offset;
    memset(s->pool + s->offset, 0, size);
    s->offset += size;

    return s->pool + s->offset - size;
}

void* stack_allocator_alloc(stack_allocator_t* s, size_t size) {
    return stack_allocator_alloc_align(s, size, 4);
}

void stack_allocator_destroy(stack_allocator_t* s) {
    if (!s)
	return;
    if (!s->pool)
	return;
    free(s->pool);
    s->pool = NULL;
    s->capacity = s->offset = 0;
}

void stack_allocator_print(stack_allocator_t* s) {
    size_t i;
    printf("cap: %ul, len: %ul\n", s->capacity, s->offset);
    for (i = 0; i < s->offset; i++)
	printf("%02x", *(s->pool + i));
    printf("\n");
    for (i = 0; i < s->offset; i++)
	printf("%u ", *(s->pool + i));
    printf("\n");
}


