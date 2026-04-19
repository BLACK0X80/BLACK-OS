#include <kernel/kernel.h>
#include <kernel/process.h>
#include <fs/vfs.h>
#include <string.h>

#define BLACK_MAX_SYSCALLS 256

typedef int32_t (*black_syscall_fn)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

static black_syscall_fn black_syscall_table[BLACK_MAX_SYSCALLS];

static int32_t black_sys_exit(uint32_t black_code, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
    (void)b; (void)c; (void)d; (void)e;
    black_process_exit((int32_t)black_code);
    return 0;
}

static int32_t black_sys_read(uint32_t black_fd, uint32_t black_buf, uint32_t black_sz, uint32_t d, uint32_t e)
{
    (void)black_fd; (void)d; (void)e;
    black_process_t *black_p = black_process_current();
    if (!black_p || black_fd >= MAX_FDS_PER_PROC || !black_p->black_fd_table[black_fd].black_in_use) return -1;
    black_vfs_node_t *black_n = black_p->black_fd_table[black_fd].black_node;
    uint32_t black_off = black_p->black_fd_table[black_fd].black_offset;
    int black_r = black_vfs_read(black_n, black_off, black_sz, (uint8_t *)black_buf);
    if (black_r > 0) black_p->black_fd_table[black_fd].black_offset += (uint32_t)black_r;
    return black_r;
}

static int32_t black_sys_write(uint32_t black_fd, uint32_t black_buf, uint32_t black_sz, uint32_t d, uint32_t e)
{
    (void)black_fd; (void)d; (void)e;
    black_process_t *black_p = black_process_current();
    if (!black_p || black_fd >= MAX_FDS_PER_PROC || !black_p->black_fd_table[black_fd].black_in_use) return -1;
    black_vfs_node_t *black_n = black_p->black_fd_table[black_fd].black_node;
    uint32_t black_off = black_p->black_fd_table[black_fd].black_offset;
    int black_r = black_vfs_write(black_n, black_off, black_sz, (uint8_t *)black_buf);
    if (black_r > 0) black_p->black_fd_table[black_fd].black_offset += (uint32_t)black_r;
    return black_r;
}

static int32_t black_sys_open(uint32_t black_path_ptr, uint32_t black_flags, uint32_t c, uint32_t d, uint32_t e)
{
    (void)c; (void)d; (void)e;
    const char *black_path = (const char *)black_path_ptr;
    black_vfs_node_t *black_n = black_vfs_lookup(black_path);
    if (!black_n) return -1;
    black_process_t *black_p = black_process_current();
    if (!black_p) return -1;
    for (int black_i = 0; black_i < MAX_FDS_PER_PROC; black_i++) {
        if (!black_p->black_fd_table[black_i].black_in_use) {
            black_p->black_fd_table[black_i].black_node = (struct black_vfs_node *)black_n;
            black_p->black_fd_table[black_i].black_offset = 0;
            black_p->black_fd_table[black_i].black_flags = black_flags;
            black_p->black_fd_table[black_i].black_in_use = 1;
            black_vfs_open(black_n, black_flags);
            return black_i;
        }
    }
    return -1;
}

static int32_t black_sys_close(uint32_t black_fd, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
    (void)b; (void)c; (void)d; (void)e;
    black_process_t *black_p = black_process_current();
    if (!black_p || black_fd >= MAX_FDS_PER_PROC || !black_p->black_fd_table[black_fd].black_in_use) return -1;
    black_vfs_close((black_vfs_node_t *)black_p->black_fd_table[black_fd].black_node);
    black_p->black_fd_table[black_fd].black_in_use = 0;
    return 0;
}

static int32_t black_sys_getpid(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
    (void)a; (void)b; (void)c; (void)d; (void)e;
    black_process_t *black_p = black_process_current();
    return black_p ? (int32_t)black_p->black_pid : -1;
}

static int32_t black_sys_fork(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
    (void)a; (void)b; (void)c; (void)d; (void)e;
    return black_process_fork();
}

static int32_t black_sys_exec(uint32_t black_path_ptr, uint32_t black_argv_ptr, uint32_t c, uint32_t d, uint32_t e)
{
    (void)c; (void)d; (void)e;
    return black_process_exec((const char *)black_path_ptr, (char *const *)black_argv_ptr);
}

static int32_t black_sys_waitpid(uint32_t black_pid, uint32_t black_status_ptr, uint32_t black_opts, uint32_t d, uint32_t e)
{
    (void)d; (void)e;
    return black_process_waitpid((int32_t)black_pid, (int32_t *)black_status_ptr, (int)black_opts);
}

static int32_t black_sys_kill(uint32_t black_pid, uint32_t black_sig, uint32_t c, uint32_t d, uint32_t e)
{
    (void)c; (void)d; (void)e;
    return black_process_signal(black_pid, (int)black_sig);
}

void black_syscall_init(void)
{
    memset(black_syscall_table, 0, sizeof(black_syscall_table));
    black_syscall_table[1] = black_sys_exit;
    black_syscall_table[3] = black_sys_read;
    black_syscall_table[4] = black_sys_write;
    black_syscall_table[5] = black_sys_open;
    black_syscall_table[6] = black_sys_close;
    black_syscall_table[2] = black_sys_fork;
    black_syscall_table[7] = black_sys_waitpid;
    black_syscall_table[11] = black_sys_exec;
    black_syscall_table[20] = black_sys_getpid;
    black_syscall_table[37] = black_sys_kill;
}

int32_t black_syscall_dispatch(uint32_t black_num, uint32_t black_a, uint32_t black_b, uint32_t black_c, uint32_t black_d, uint32_t black_e)
{
    if (black_num >= BLACK_MAX_SYSCALLS || !black_syscall_table[black_num]) return -1;
    return black_syscall_table[black_num](black_a, black_b, black_c, black_d, black_e);
}
