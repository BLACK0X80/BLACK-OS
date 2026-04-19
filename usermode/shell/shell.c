#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    print("=== BlackOS Matrix Shell (Ring 3) ===\n");
    print("Type 'exit' to quit.\n");

    char black_cmd[256];
    while (1) {
        print("\nblack> ");
        int black_pos = 0;
        char black_c;
        while (1) {
            int black_r = read(0, &black_c, 1);
            if (black_r <= 0) continue;
            if (black_c == '\n' || black_c == '\r') {
                black_cmd[black_pos] = '\0';
                print_char('\n');
                break;
            } else if (black_c == '\b' || black_c == 127) {
                if (black_pos > 0) {
                    black_pos--;
                    print("\b \b");
                }
            } else {
                black_cmd[black_pos++] = black_c;
                print_char(black_c);
            }
        }

        if (black_pos == 0) continue;

        if (strcmp(black_cmd, "exit") == 0) {
            print("Session terminated.\n");
            exit(0);
        }

        int black_pid = fork();
        if (black_pid == 0) {
            char *black_args[] = { black_cmd, NULL };
            exec(black_cmd, black_args);
            print("Command not found.\n");
            exit(1);
        } else if (black_pid > 0) {
            int black_status;
            waitpid(black_pid, &black_status, 0);
        } else {
            print("Fork failed.\n");
        }
    }
    return 0;
}
