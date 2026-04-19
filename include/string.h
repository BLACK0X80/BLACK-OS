#ifndef _BLACK_STRING_H
#define _BLACK_STRING_H

#include <stdint.h>

void *memset(void *black_dst, int black_val, size_t black_n);
void *memcpy(void *black_dst, const void *black_src, size_t black_n);
void *memmove(void *black_dst, const void *black_src, size_t black_n);
int   memcmp(const void *black_a, const void *black_b, size_t black_n);

size_t strlen(const char *black_s);
char  *strcpy(char *black_dst, const char *black_src);
char  *strncpy(char *black_dst, const char *black_src, size_t black_n);
int    strcmp(const char *black_a, const char *black_b);
int    strncmp(const char *black_a, const char *black_b, size_t black_n);
char  *strcat(char *black_dst, const char *black_src);
char  *strncat(char *black_dst, const char *black_src, size_t black_n);
char  *strchr(const char *black_s, int black_c);
char  *strrchr(const char *black_s, int black_c);
char  *strstr(const char *black_haystack, const char *black_needle);

#endif
