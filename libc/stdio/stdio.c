#include <stdio.h>
#include <kernel/kernel.h>

int printf(const char *black_fmt, ...) {
    (void)black_fmt;
    return 0;
}
