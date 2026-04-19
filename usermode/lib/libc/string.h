#ifndef _BLACK_STRING_H
#define _BLACK_STRING_H

#include <stddef.h>

size_t strlen(const char *black_str);
int strcmp(const char *black_s1, const char *black_s2);
void *memcpy(void *black_dst, const void *black_src, size_t black_n);
void *memset(void *black_dst, int black_c, size_t black_n);

#endif
