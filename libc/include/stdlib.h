#ifndef _BLACK_STDLIB_H
#define _BLACK_STDLIB_H

#include <stddef.h>

void *malloc(size_t black_size);
void free(void *black_ptr);
int rand(void);
void srand(unsigned int black_seed);

#endif
