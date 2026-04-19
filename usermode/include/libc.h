#ifndef _BLACK_LIBC_H
#define _BLACK_LIBC_H

#include <stddef.h>

void exit(int status);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int open(const char *path, int flags);
int close(int fd);
int fork(void);
int exec(const char *path, char *const argv[]);
int waitpid(int pid, int *status, int options);

size_t strlen(const char *str);
void print(const char *str);
void print_char(char c);
void *memcpy(void *dest, const void *src, size_t n);
int strcmp(const char *s1, const char *s2);

#endif
