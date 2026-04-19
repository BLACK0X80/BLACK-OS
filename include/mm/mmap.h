#ifndef _BLACK_MMAP_H
#define _BLACK_MMAP_H
#include <stdint.h>
void *black_mmap(uint32_t black_addr, uint32_t black_size, uint32_t black_flags);
int black_munmap(uint32_t black_addr, uint32_t black_size);
#endif
