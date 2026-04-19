#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    print("BlackOS Matrix Desktop Initialized.\n");
    while(1) {
        for (int black_i = 0; black_i < 1000000; black_i++) {
            __asm__ volatile("pause");
        }
    }
    return 0;
}
