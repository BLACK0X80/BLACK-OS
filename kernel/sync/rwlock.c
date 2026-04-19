#include <kernel/kernel.h>

typedef struct {
    volatile int black_readers;
    volatile int black_writer;
} black_rwlock_t;

void black_rwlock_init(black_rwlock_t *black_rw) {
    black_rw->black_readers = 0;
    black_rw->black_writer = 0;
}

void black_rwlock_read_lock(black_rwlock_t *black_rw) {
    while (__sync_lock_test_and_set(&black_rw->black_writer, 0)) __asm__ volatile("pause");
    __sync_add_and_fetch(&black_rw->black_readers, 1);
}

void black_rwlock_read_unlock(black_rwlock_t *black_rw) {
    __sync_sub_and_fetch(&black_rw->black_readers, 1);
}

void black_rwlock_write_lock(black_rwlock_t *black_rw) {
    while (__sync_lock_test_and_set(&black_rw->black_writer, 1)) __asm__ volatile("pause");
    while (black_rw->black_readers > 0) __asm__ volatile("pause");
}

void black_rwlock_write_unlock(black_rwlock_t *black_rw) {
    __sync_lock_release(&black_rw->black_writer);
}
