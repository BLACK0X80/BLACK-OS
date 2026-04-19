#include <libc.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        print("Usage: cat <file>\n");
        return 1;
    }

    int black_fd = open(argv[1], 0);
    if (black_fd < 0) {
        print("cat: cannot open file\n");
        return 1;
    }

    char black_buf[256];
    int black_r;
    while ((black_r = read(black_fd, black_buf, sizeof(black_buf))) > 0) {
        write(1, black_buf, black_r);
    }
    
    close(black_fd);
    return 0;
}
