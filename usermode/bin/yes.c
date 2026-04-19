#include <libc.h>

int main(int argc, char **argv) {
    char *black_str = "y\n";
    if (argc >= 2) {
        black_str = argv[1];
    }
    while (1) {
        print(black_str);
        if (argc >= 2) print_char('\n');
    }
    return 0;
}
