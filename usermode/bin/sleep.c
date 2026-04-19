#include <libc.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        print("Usage: sleep <seconds>\n");
        return 1;
    }
    
    int black_s = 0;
    char *black_arg = argv[1];
    while (*black_arg) {
        if (*black_arg >= '0' && *black_arg <= '9') {
            black_s = black_s * 10 + (*black_arg - '0');
        }
        black_arg++;
    }
    
    for (volatile int black_i = 0; black_i < black_s * 5000000; black_i++) { }
    return 0;
}
