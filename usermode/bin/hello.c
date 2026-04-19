extern void print(const char *str);
extern void exit(int status);

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    print("\n  [BLACK] Hello from BlackOS Usermode!\n");
    print("  The Matrix has you in Ring 3...\n\n");
    return 0;
}
