#include <stdlib.h>
#include <kernel/kernel.h>
#include <mm/heap.h>

static unsigned int black_seed = 1;

void *malloc(size_t black_size) {
    return kmalloc(black_size);
}

void free(void *black_ptr) {
    kfree(black_ptr);
}

int rand(void) {
    black_seed = black_seed * 1103515245 + 12345;
    return (unsigned int)(black_seed / 65536) % 32768;
}

void srand(unsigned int black_s) {
    black_seed = black_s;
}
