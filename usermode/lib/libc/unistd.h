#ifndef _BLACK_UNISTD_H
#define _BLACK_UNISTD_H

#include <stddef.h>
#include <stdint.h>

int read(int black_fd, void *black_buf, size_t black_count);
int write(int black_fd, const void *black_buf, size_t black_count);
int close(int black_fd);
int fork(void);
int exec(const char *black_path, char *const black_argv[]);
int getpid(void);

#endif
