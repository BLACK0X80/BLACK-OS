#include <stdint.h>
#include <stddef.h>

#define SYS_EXIT  1
#define SYS_READ  3
#define SYS_WRITE 4
#define SYS_OPEN  5
#define SYS_CLOSE 6
#define SYS_FORK  2
#define SYS_EXEC  11
#define SYS_WAITPID 7

static inline int syscall0(int num) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num) : "memory");
    return ret;
}

static inline int syscall1(int num, int arg1) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1) : "memory");
    return ret;
}

static inline int syscall2(int num, int arg1, int arg2) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2) : "memory");
    return ret;
}

static inline int syscall3(int num, int arg1, int arg2, int arg3) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3) : "memory");
    return ret;
}

void exit(int status) { syscall1(SYS_EXIT, status); while(1); }
int read(int fd, void *buf, size_t count) { return syscall3(SYS_READ, fd, (int)buf, count); }
int write(int fd, const void *buf, size_t count) { return syscall3(SYS_WRITE, fd, (int)buf, count); }
int open(const char *path, int flags) { return syscall2(SYS_OPEN, (int)path, flags); }
int close(int fd) { return syscall1(SYS_CLOSE, fd); }
int fork(void) { return syscall0(SYS_FORK); }
int exec(const char *path, char *const argv[]) { return syscall2(SYS_EXEC, (int)path, (int)argv); }
int waitpid(int pid, int *status, int options) { return syscall3(SYS_WAITPID, pid, (int)status, options); }

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void print(const char *str) { write(1, str, strlen(str)); }
void print_char(char c) { write(1, &c, 1); }

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest; const char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
