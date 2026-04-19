#include <kernel/kernel.h>
#include <sync/spinlock.h>
#include <string.h>

#define BLACK_MAX_PIPES     32
#define BLACK_PIPE_BUF_SIZE 4096
#define BLACK_MAX_MSG_QUEUES 16
#define BLACK_MAX_MESSAGES   64
#define BLACK_MAX_SHM        16

typedef struct {
    uint8_t black_buffer[BLACK_PIPE_BUF_SIZE];
    uint32_t black_read_pos;
    uint32_t black_write_pos;
    uint32_t black_count;
    int black_readers;
    int black_writers;
    int black_active;
    black_spinlock_t black_lock;
} black_pipe_t;

typedef struct {
    void *black_data;
    uint32_t black_size;
} black_msg_t;

typedef struct {
    black_msg_t black_messages[BLACK_MAX_MESSAGES];
    uint32_t black_head;
    uint32_t black_tail;
    uint32_t black_count;
    int black_active;
    char black_name[32];
    black_spinlock_t black_lock;
} black_msg_queue_t;

typedef struct {
    uint32_t black_key;
    void *black_addr;
    uint32_t black_size;
    int black_ref_count;
    int black_active;
} black_shm_t;

static black_pipe_t black_pipes[BLACK_MAX_PIPES];
static black_msg_queue_t black_msg_queues[BLACK_MAX_MSG_QUEUES];
static black_shm_t black_shm_segments[BLACK_MAX_SHM];

void black_ipc_init(void)
{
    memset(black_pipes, 0, sizeof(black_pipes));
    memset(black_msg_queues, 0, sizeof(black_msg_queues));
    memset(black_shm_segments, 0, sizeof(black_shm_segments));
}

int black_pipe_create(void)
{
    for (int black_i = 0; black_i < BLACK_MAX_PIPES; black_i++) {
        if (!black_pipes[black_i].black_active) {
            memset(&black_pipes[black_i], 0, sizeof(black_pipe_t));
            black_pipes[black_i].black_active = 1;
            black_pipes[black_i].black_readers = 1;
            black_pipes[black_i].black_writers = 1;
            return black_i;
        }
    }
    return -1;
}

int black_pipe_read(int black_id, void *black_buf, uint32_t black_sz)
{
    if (black_id < 0 || black_id >= BLACK_MAX_PIPES || !black_pipes[black_id].black_active) return -1;
    black_pipe_t *black_p = &black_pipes[black_id];
    uint32_t black_fl;
    black_spinlock_irq_save(&black_p->black_lock, &black_fl);
    uint32_t black_read = 0;
    uint8_t *black_dst = (uint8_t *)black_buf;
    while (black_read < black_sz && black_p->black_count > 0) {
        black_dst[black_read++] = black_p->black_buffer[black_p->black_read_pos];
        black_p->black_read_pos = (black_p->black_read_pos + 1) % BLACK_PIPE_BUF_SIZE;
        black_p->black_count--;
    }
    black_spinlock_irq_restore(&black_p->black_lock, black_fl);
    return (int)black_read;
}

int black_pipe_write(int black_id, const void *black_buf, uint32_t black_sz)
{
    if (black_id < 0 || black_id >= BLACK_MAX_PIPES || !black_pipes[black_id].black_active) return -1;
    black_pipe_t *black_p = &black_pipes[black_id];
    uint32_t black_fl;
    black_spinlock_irq_save(&black_p->black_lock, &black_fl);
    uint32_t black_written = 0;
    const uint8_t *black_src = (const uint8_t *)black_buf;
    while (black_written < black_sz && black_p->black_count < BLACK_PIPE_BUF_SIZE) {
        black_p->black_buffer[black_p->black_write_pos] = black_src[black_written++];
        black_p->black_write_pos = (black_p->black_write_pos + 1) % BLACK_PIPE_BUF_SIZE;
        black_p->black_count++;
    }
    black_spinlock_irq_restore(&black_p->black_lock, black_fl);
    return (int)black_written;
}

void black_pipe_close(int black_id)
{
    if (black_id >= 0 && black_id < BLACK_MAX_PIPES)
        black_pipes[black_id].black_active = 0;
}

int black_msgq_create(const char *black_name)
{
    for (int black_i = 0; black_i < BLACK_MAX_MSG_QUEUES; black_i++) {
        if (!black_msg_queues[black_i].black_active) {
            memset(&black_msg_queues[black_i], 0, sizeof(black_msg_queue_t));
            strncpy(black_msg_queues[black_i].black_name, black_name, 31);
            black_msg_queues[black_i].black_active = 1;
            return black_i;
        }
    }
    return -1;
}

int black_msgq_send(int black_id, void *black_data, uint32_t black_sz)
{
    if (black_id < 0 || black_id >= BLACK_MAX_MSG_QUEUES || !black_msg_queues[black_id].black_active) return -1;
    black_msg_queue_t *black_q = &black_msg_queues[black_id];
    if (black_q->black_count >= BLACK_MAX_MESSAGES) return -1;
    uint32_t black_fl;
    black_spinlock_irq_save(&black_q->black_lock, &black_fl);
    void *black_copy = kmalloc(black_sz);
    if (!black_copy) { black_spinlock_irq_restore(&black_q->black_lock, black_fl); return -1; }
    memcpy(black_copy, black_data, black_sz);
    black_q->black_messages[black_q->black_head].black_data = black_copy;
    black_q->black_messages[black_q->black_head].black_size = black_sz;
    black_q->black_head = (black_q->black_head + 1) % BLACK_MAX_MESSAGES;
    black_q->black_count++;
    black_spinlock_irq_restore(&black_q->black_lock, black_fl);
    return 0;
}

int black_msgq_recv(int black_id, void *black_buf, uint32_t black_max)
{
    if (black_id < 0 || black_id >= BLACK_MAX_MSG_QUEUES || !black_msg_queues[black_id].black_active) return -1;
    black_msg_queue_t *black_q = &black_msg_queues[black_id];
    if (black_q->black_count == 0) return 0;
    uint32_t black_fl;
    black_spinlock_irq_save(&black_q->black_lock, &black_fl);
    black_msg_t *black_m = &black_q->black_messages[black_q->black_tail];
    uint32_t black_copy_sz = black_m->black_size < black_max ? black_m->black_size : black_max;
    memcpy(black_buf, black_m->black_data, black_copy_sz);
    kfree(black_m->black_data);
    black_q->black_tail = (black_q->black_tail + 1) % BLACK_MAX_MESSAGES;
    black_q->black_count--;
    black_spinlock_irq_restore(&black_q->black_lock, black_fl);
    return (int)black_copy_sz;
}

int black_shm_create(uint32_t black_key, uint32_t black_sz)
{
    for (int black_i = 0; black_i < BLACK_MAX_SHM; black_i++) {
        if (!black_shm_segments[black_i].black_active) {
            void *black_mem = kcalloc(1, black_sz);
            if (!black_mem) return -1;
            black_shm_segments[black_i].black_key = black_key;
            black_shm_segments[black_i].black_addr = black_mem;
            black_shm_segments[black_i].black_size = black_sz;
            black_shm_segments[black_i].black_ref_count = 1;
            black_shm_segments[black_i].black_active = 1;
            return black_i;
        }
    }
    return -1;
}

void *black_shm_attach(int black_id)
{
    if (black_id >= 0 && black_id < BLACK_MAX_SHM && black_shm_segments[black_id].black_active) {
        black_shm_segments[black_id].black_ref_count++;
        return black_shm_segments[black_id].black_addr;
    }
    return NULL;
}

void black_shm_detach(int black_id)
{
    if (black_id >= 0 && black_id < BLACK_MAX_SHM && black_shm_segments[black_id].black_active) {
        if (--black_shm_segments[black_id].black_ref_count <= 0) {
            kfree(black_shm_segments[black_id].black_addr);
            black_shm_segments[black_id].black_active = 0;
        }
    }
}
