#include <kernel/kernel.h>

int32_t black_syscall_fs_open(const char *black_path, int black_flags) {
    (void)black_path; (void)black_flags;
    return -1;
}

int32_t black_syscall_fs_read(int black_fd, void *black_buf, uint32_t black_size) {
    (void)black_fd; (void)black_buf; (void)black_size;
    return -1;
}

int32_t black_syscall_fs_write(int black_fd, const void *black_buf, uint32_t black_size) {
    (void)black_fd; (void)black_buf; (void)black_size;
    return -1;
}

int32_t black_syscall_fs_close(int black_fd) {
    (void)black_fd;
    return 0;
}
