#include <string.h>
#include <stdint.h>

void *memset(void *black_dst, int black_val, size_t black_n)
{
    uint8_t *black_d = (uint8_t *)black_dst;
    while (black_n--) *black_d++ = (uint8_t)black_val;
    return black_dst;
}

void *memcpy(void *black_dst, const void *black_src, size_t black_n)
{
    uint8_t *black_d = (uint8_t *)black_dst;
    const uint8_t *black_s = (const uint8_t *)black_src;
    while (black_n--) *black_d++ = *black_s++;
    return black_dst;
}

void *memmove(void *black_dst, const void *black_src, size_t black_n)
{
    uint8_t *black_d = (uint8_t *)black_dst;
    const uint8_t *black_s = (const uint8_t *)black_src;
    if (black_d < black_s) {
        while (black_n--) *black_d++ = *black_s++;
    } else {
        black_d += black_n;
        black_s += black_n;
        while (black_n--) *(--black_d) = *(--black_s);
    }
    return black_dst;
}

int memcmp(const void *black_a, const void *black_b, size_t black_n)
{
    const uint8_t *black_pa = (const uint8_t *)black_a;
    const uint8_t *black_pb = (const uint8_t *)black_b;
    while (black_n--) {
        if (*black_pa != *black_pb) return *black_pa - *black_pb;
        black_pa++;
        black_pb++;
    }
    return 0;
}

size_t strlen(const char *black_s)
{
    size_t black_len = 0;
    while (black_s[black_len]) black_len++;
    return black_len;
}

char *strcpy(char *black_dst, const char *black_src)
{
    char *black_d = black_dst;
    while ((*black_d++ = *black_src++));
    return black_dst;
}

char *strncpy(char *black_dst, const char *black_src, size_t black_n)
{
    size_t black_i;
    for (black_i = 0; black_i < black_n && black_src[black_i]; black_i++)
        black_dst[black_i] = black_src[black_i];
    for (; black_i < black_n; black_i++)
        black_dst[black_i] = '\0';
    return black_dst;
}

int strcmp(const char *black_a, const char *black_b)
{
    while (*black_a && *black_a == *black_b) { black_a++; black_b++; }
    return *(unsigned char *)black_a - *(unsigned char *)black_b;
}

int strncmp(const char *black_a, const char *black_b, size_t black_n)
{
    while (black_n && *black_a && *black_a == *black_b) { black_a++; black_b++; black_n--; }
    if (black_n == 0) return 0;
    return *(unsigned char *)black_a - *(unsigned char *)black_b;
}

char *strcat(char *black_dst, const char *black_src)
{
    char *black_d = black_dst;
    while (*black_d) black_d++;
    while ((*black_d++ = *black_src++));
    return black_dst;
}

char *strncat(char *black_dst, const char *black_src, size_t black_n)
{
    char *black_d = black_dst;
    while (*black_d) black_d++;
    while (black_n-- && *black_src) *black_d++ = *black_src++;
    *black_d = '\0';
    return black_dst;
}

char *strchr(const char *black_s, int black_c)
{
    while (*black_s) {
        if (*black_s == (char)black_c) return (char *)black_s;
        black_s++;
    }
    if (black_c == 0) return (char *)black_s;
    return NULL;
}

char *strrchr(const char *black_s, int black_c)
{
    const char *black_last = NULL;
    while (*black_s) {
        if (*black_s == (char)black_c) black_last = black_s;
        black_s++;
    }
    if (black_c == 0) return (char *)black_s;
    return (char *)black_last;
}

char *strstr(const char *black_haystack, const char *black_needle)
{
    size_t black_nlen = strlen(black_needle);
    if (black_nlen == 0) return (char *)black_haystack;
    while (*black_haystack) {
        if (strncmp(black_haystack, black_needle, black_nlen) == 0)
            return (char *)black_haystack;
        black_haystack++;
    }
    return NULL;
}
