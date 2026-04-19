#include <kernel/kernel.h>

typedef struct {
    volatile int black_count;
} black_waitqueue_t;

void black_waitqueue_init(black_waitqueue_t *black_wq) {
    black_wq->black_count = 0;
}

void black_waitqueue_sleep(black_waitqueue_t *black_wq) {
    __sync_add_and_fetch(&black_wq->black_count, 1);
    while (black_wq->black_count > 0) __asm__ volatile("pause");
}

void black_waitqueue_wake(black_waitqueue_t *black_wq) {
    __sync_lock_release(&black_wq->black_count);
}
