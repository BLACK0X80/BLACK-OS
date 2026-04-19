#include <stdlib.h>
#include <syscall.h>

void exit(int black_code) {
    black_syscall1(1, black_code);
    while(1);
}

int atoi(const char *black_str) {
    int black_result = 0;
    int black_sign = 1;
    if (*black_str == '-') { black_sign = -1; black_str++; }
    while (*black_str >= '0' && *black_str <= '9') {
        black_result = black_result * 10 + (*black_str - '0');
        black_str++;
    }
    return black_sign * black_result;
}
