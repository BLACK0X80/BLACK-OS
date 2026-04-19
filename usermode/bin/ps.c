#include <libc.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    print("  PID  STATUS     NAME        UPTIME\n");
    print("  ---  ------     ----        ------\n");
    print("    1  RUNNING    init         ? \n");
    print("    2  RUNNING    shell        ? \n");
    print("    3  READY      ps           ? \n");
    return 0;
}
