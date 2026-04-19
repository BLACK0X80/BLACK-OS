#ifndef _BLACK_KERNEL_H
#define _BLACK_KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define BLACK_VERSION_MAJOR  1
#define BLACK_VERSION_MINOR  0
#define BLACK_VERSION_PATCH  0
#define BLACK_VERSION_STRING "1.0.0"
#define BLACK_CODENAME       "PHANTOM"
#define BLACK_OS_NAME        "BlackOS"

#define KERNEL_VMA           0xC0000000

#define BLACK_ALIGN(black_x, black_a) (((black_x) + ((black_a) - 1)) & ~((black_a) - 1))
#define BLACK_MIN(black_a, black_b)   ((black_a) < (black_b) ? (black_a) : (black_b))
#define BLACK_MAX(black_a, black_b)   ((black_a) > (black_b) ? (black_a) : (black_b))
#define BLACK_ARRAY_SIZE(black_arr)   (sizeof(black_arr) / sizeof((black_arr)[0]))

#define BLACK_PACKED    __attribute__((packed))
#define BLACK_NORETURN  __attribute__((noreturn))
#define BLACK_UNUSED    __attribute__((unused))
#define BLACK_ALIGNED(n) __attribute__((aligned(n)))

static inline void black_cli(void) { __asm__ volatile("cli"); }
static inline void black_sti(void) { __asm__ volatile("sti"); }
static inline void black_hlt(void) { __asm__ volatile("hlt"); }

static inline void black_outb(uint16_t black_port, uint8_t black_val) {
    __asm__ volatile("outb %0, %1" : : "a"(black_val), "Nd"(black_port));
}

static inline uint8_t black_inb(uint16_t black_port) {
    uint8_t black_val;
    __asm__ volatile("inb %1, %0" : "=a"(black_val) : "Nd"(black_port));
    return black_val;
}

static inline void black_outw(uint16_t black_port, uint16_t black_val) {
    __asm__ volatile("outw %0, %1" : : "a"(black_val), "Nd"(black_port));
}

static inline uint16_t black_inw(uint16_t black_port) {
    uint16_t black_val;
    __asm__ volatile("inw %1, %0" : "=a"(black_val) : "Nd"(black_port));
    return black_val;
}

static inline void black_outl(uint16_t black_port, uint32_t black_val) {
    __asm__ volatile("outl %0, %1" : : "a"(black_val), "Nd"(black_port));
}

static inline uint32_t black_inl(uint16_t black_port) {
    uint32_t black_val;
    __asm__ volatile("inl %1, %0" : "=a"(black_val) : "Nd"(black_port));
    return black_val;
}

static inline void black_io_wait(void) {
    black_outb(0x80, 0);
}

static inline uint64_t black_rdtsc(void) {
    uint32_t black_lo, black_hi;
    __asm__ volatile("rdtsc" : "=a"(black_lo), "=d"(black_hi));
    return ((uint64_t)black_hi << 32) | black_lo;
}

static inline void black_cpuid(uint32_t black_leaf, uint32_t *black_eax,
                                uint32_t *black_ebx, uint32_t *black_ecx,
                                uint32_t *black_edx) {
    __asm__ volatile("cpuid"
        : "=a"(*black_eax), "=b"(*black_ebx), "=c"(*black_ecx), "=d"(*black_edx)
        : "a"(black_leaf));
}

void *kmalloc(size_t black_size);
void  kfree(void *black_ptr);
void *kcalloc(size_t black_count, size_t black_size);
void *krealloc(void *black_ptr, size_t black_size);

void black_panic(const char *black_msg) BLACK_NORETURN;
void black_klog(int black_level, const char *black_subsys, const char *black_fmt, ...);

#define BLACK_LOG_DEBUG  0
#define BLACK_LOG_INFO   1
#define BLACK_LOG_WARN   2
#define BLACK_LOG_ERROR  3
#define BLACK_LOG_FATAL  4

#define KLOG_D(black_sub, black_fmt, ...) black_klog(BLACK_LOG_DEBUG, black_sub, black_fmt, ##__VA_ARGS__)
#define KLOG_I(black_sub, black_fmt, ...) black_klog(BLACK_LOG_INFO,  black_sub, black_fmt, ##__VA_ARGS__)
#define KLOG_W(black_sub, black_fmt, ...) black_klog(BLACK_LOG_WARN,  black_sub, black_fmt, ##__VA_ARGS__)
#define KLOG_E(black_sub, black_fmt, ...) black_klog(BLACK_LOG_ERROR, black_sub, black_fmt, ##__VA_ARGS__)

extern uint32_t __stack_chk_guard;

#define outb black_outb
#define inb  black_inb
#define outw black_outw
#define inw  black_inw
#define outl black_outl
#define inl  black_inl
#define cli  black_cli
#define sti  black_sti

#endif
