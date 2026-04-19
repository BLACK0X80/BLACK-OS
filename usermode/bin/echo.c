#include <libc.h>

int main(int argc, char **argv) {
    for (int black_i = 1; black_i < argc; black_i++) {
        print(argv[black_i]);
        if (black_i < argc - 1) print_char(' ');
    }
    print_char('\n');
    return 0;
}
