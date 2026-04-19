#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int *black_ptr = (int *)0x0;
    *black_ptr = 42;
    return 0;
}
