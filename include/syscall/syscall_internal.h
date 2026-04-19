#ifndef _BLACK_SYSCALL_INTERNAL_H
#define _BLACK_SYSCALL_INTERNAL_H
#include <stdint.h>
int32_t black_syscall_fs_open(const char *black_path, int black_flags);
int32_t black_syscall_fs_read(int black_fd, void *black_buf, uint32_t black_size);
int32_t black_syscall_fs_write(int black_fd, const void *black_buf, uint32_t black_size);
int32_t black_syscall_fs_close(int black_fd);
int32_t black_syscall_gui_create_window(const char *black_title, int black_x, int black_y, int black_w, int black_h);
int32_t black_syscall_net_socket(int black_type, int black_proto);
int32_t black_syscall_net_send(int black_fd, const void *black_buf, uint32_t black_len);
int32_t black_syscall_net_recv(int black_fd, void *black_buf, uint32_t black_len);
int32_t black_syscall_proc_fork(void);
int32_t black_syscall_proc_exec(const char *black_path);
int32_t black_syscall_proc_exit(int black_code);
int32_t black_syscall_proc_waitpid(int black_pid);
int32_t black_syscall_proc_getpid(void);
#endif
