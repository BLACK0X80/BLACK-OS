#ifndef _BLACK_IPC_H
#define _BLACK_IPC_H
#include <stdint.h>
void black_ipc_init(void);
int black_pipe_create(int *black_fds);
int black_pipe_read(int black_fd, void *black_buf, uint32_t black_size);
int black_pipe_write(int black_fd, const void *black_buf, uint32_t black_size);
#endif
