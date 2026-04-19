#ifndef _BLACK_HEAP_H
#define _BLACK_HEAP_H

#include <stdint.h>

void black_heap_init(uint32_t black_start, uint32_t black_size);
void *black_heap_alloc(size_t black_size);
void  black_heap_free(void *black_ptr);
void *black_heap_calloc(size_t black_count, size_t black_size);
void *black_heap_realloc(void *black_ptr, size_t black_size);
uint32_t black_heap_get_used(void);
uint32_t black_heap_get_free(void);

#define heap_init black_heap_init

#endif
