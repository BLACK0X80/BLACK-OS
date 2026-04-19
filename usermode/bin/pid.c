#include <libc.h>

extern int getpid(void);

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int black_p = getpid();
    if (black_p > 0) print("PID: ACTIVE\n");
    return 0;
}
