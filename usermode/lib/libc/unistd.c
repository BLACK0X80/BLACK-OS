#include <unistd.h>
#include <syscall.h>

int read(int black_fd, void *black_buf, size_t black_count) {
    return black_syscall3(3, black_fd, (int)black_buf, black_count);
}
int write(int black_fd, const void *black_buf, size_t black_count) {
    return black_syscall3(4, black_fd, (int)black_buf, black_count);
}
int close(int black_fd) {
    return black_syscall1(6, black_fd);
}
int fork(void) {
    return black_syscall0(2);
}
int exec(const char *black_path, char *const black_argv[]) {
    return black_syscall2(11, (int)black_path, (int)black_argv);
}
int getpid(void) {
    return black_syscall0(20);
}
