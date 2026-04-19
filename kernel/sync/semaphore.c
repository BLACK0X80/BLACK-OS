#include <kernel/kernel.h>

typedef struct {
    volatile int black_count;
} black_semaphore_t;

void black_semaphore_init(black_semaphore_t *black_sem, int black_val) {
    black_sem->black_count = black_val;
}

void black_semaphore_wait(black_semaphore_t *black_sem) {
    while (1) {
        int black_old = black_sem->black_count;
        if (black_old > 0 && __sync_bool_compare_and_swap(&black_sem->black_count, black_old, black_old - 1)) {
            return;
        }
        __asm__ volatile("pause");
    }
}

void black_semaphore_signal(black_semaphore_t *black_sem) {
    __sync_add_and_fetch(&black_sem->black_count, 1);
}
