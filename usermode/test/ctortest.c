#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    print("CTOR OK\n");
    return 0;
}
