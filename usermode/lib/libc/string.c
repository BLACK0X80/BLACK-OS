#include <string.h>

size_t strlen(const char *black_str) {
    size_t black_len = 0;
    while (black_str[black_len]) black_len++;
    return black_len;
}

int strcmp(const char *black_s1, const char *black_s2) {
    while (*black_s1 && (*black_s1 == *black_s2)) {
        black_s1++;
        black_s2++;
    }
    return *(const unsigned char*)black_s1 - *(const unsigned char*)black_s2;
}

void *memcpy(void *black_dst, const void *black_src, size_t black_n) {
    char *black_d = black_dst;
    const char *black_s = black_src;
    while (black_n--) *black_d++ = *black_s++;
    return black_dst;
}

void *memset(void *black_dst, int black_c, size_t black_n) {
    char *black_d = black_dst;
    while (black_n--) *black_d++ = black_c;
    return black_dst;
}
