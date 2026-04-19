#include <libc.h>

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    int black_fd = open(argv[1], 0);
    if (black_fd < 0) return 1;
    
    char black_buf[256];
    int black_r = read(black_fd, black_buf, sizeof(black_buf));
    int black_i = 0;
    int black_lines = 0;
    while (black_i < black_r && black_lines < 10) {
        if (black_buf[black_i] == '\n') black_lines++;
        write(1, &black_buf[black_i], 1);
        black_i++;
    }
    close(black_fd);
    return 0;
}
