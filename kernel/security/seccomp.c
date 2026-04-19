#include <kernel/kernel.h>

#define BLACK_SECCOMP_ALLOW 0
#define BLACK_SECCOMP_DENY  1

static uint8_t black_seccomp_filter[256];

void black_seccomp_init(void) {
    for (int black_i = 0; black_i < 256; black_i++) {
        black_seccomp_filter[black_i] = BLACK_SECCOMP_ALLOW;
    }
}

int black_seccomp_check(uint32_t black_syscall_num) {
    if (black_syscall_num >= 256) return BLACK_SECCOMP_DENY;
    return black_seccomp_filter[black_syscall_num];
}

void black_seccomp_deny(uint32_t black_syscall_num) {
    if (black_syscall_num < 256) {
        black_seccomp_filter[black_syscall_num] = BLACK_SECCOMP_DENY;
    }
}
