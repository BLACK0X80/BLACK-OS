#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    print("INIT PROCESS RUNNING IN RING 3\n");
    int black_pid = fork();
    if (black_pid == 0) {
        char *black_args[] = { "/bin/shell", NULL };
        exec("/bin/shell", black_args);
        print("INIT: failed to launch shell\n");
        exit(1);
    } else {
        int black_status;
        while (1) {
            waitpid(black_pid, &black_status, 0);
        }
    }
    return 0;
}
