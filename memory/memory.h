#ifndef MEMORY_H
#define MEMORY_H
#include <stdlib.h>

int power_of_two(size_t value);
typedef struct small_stack_allocator_s {
    unsigned char* pool;
    size_t capacity;
    size_t offset;
} small_stack_allocator_t;

int small_stack_allocator_create(small_stack_allocator_t* s, size_t cap);
void small_stack_allocator_free(small_stack_allocator_t* s, void* ptr);
void* small_stack_allocator_alloc_align(small_stack_allocator_t* s, size_t size, size_t alignment);
void* small_stack_allocator_alloc(small_stack_allocator_t* s, size_t size);
void small_stack_allocator_destroy(small_stack_allocator_t* s);
void small_stack_allocator_print(small_stack_allocator_t* s);

typedef struct stack_allocator_s {
    unsigned char* pool;
    size_t capacity;
    size_t offset;
    size_t prev_offset;
} stack_allocator_t;

// every allocation has at least 16B of overhead
int stack_allocator_create(stack_allocator_t* s, size_t cap);
void stack_allocator_free(stack_allocator_t* s, void* ptr);
void* stack_allocator_alloc_align(stack_allocator_t* s, size_t size, size_t alignment);
void* stack_allocator_alloc(stack_allocator_t* s, size_t size);
void stack_allocator_destroy(stack_allocator_t* s);
void stack_allocator_print(stack_allocator_t* s);

typedef struct arena_allocator_s {
    unsigned char* pool;
    size_t capacity;
    size_t current_offset;
    int backed;
} arena_allocator_t;

int arena_allocator_create(arena_allocator_t* a, size_t cap);
int arena_allocator_create_backed(arena_allocator_t* a, unsigned char* buffer, size_t cap);
void* arena_allocator_alloc(arena_allocator_t* a, size_t size);
void* arena_allocator_alloc_align(arena_allocator_t* a, size_t size, size_t alignment);
void arena_allocator_destroy(arena_allocator_t* a);
void arena_allocator_reset(arena_allocator_t* a);
void arena_allocator_print(arena_allocator_t* a);

typedef struct pool_allocator_block_s {
    struct pool_allocator_block_s* next;
    /* data */
} pool_allocator_block_t;

typedef struct pool_allocator_s {
    unsigned char* pool;
    size_t capacity, block_size;
    pool_allocator_block_t *fl_head;
    int backed;
} pool_allocator_t;

int pool_allocator_create(pool_allocator_t* p, size_t cap, size_t block_size, size_t block_alignment);
int pool_allocator_create_backed(pool_allocator_t* p, unsigned char* buffer, size_t buffer_size, size_t cap, size_t block_size, size_t block_alignment);
void pool_allocator_destroy(pool_allocator_t* p);
void* pool_allocator_alloc(pool_allocator_t* p);
void pool_allocator_free(pool_allocator_t* p, void* ptr);
void pool_allocator_free_all(pool_allocator_t* p);


#endif
