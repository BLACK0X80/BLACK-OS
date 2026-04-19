#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int black_fd = open("/proc/uptime", 0);
    if (black_fd >= 0) {
        char black_buf[64];
        int black_r = read(black_fd, black_buf, sizeof(black_buf));
        if (black_r > 0) write(1, black_buf, black_r);
        close(black_fd);
    }
    return 0;
}
