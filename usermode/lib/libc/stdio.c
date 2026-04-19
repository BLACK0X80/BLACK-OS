#include <stdio.h>
#include <syscall.h>

static int black_strlen(const char *black_s) {
    int black_l = 0;
    while (black_s[black_l]) black_l++;
    return black_l;
}

void print(const char *black_str) {
    black_syscall3(4, 1, (int)black_str, black_strlen(black_str));
}

void print_char(char black_c) {
    black_syscall3(4, 1, (int)&black_c, 1);
}
