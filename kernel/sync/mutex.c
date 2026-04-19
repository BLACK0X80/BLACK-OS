#include <kernel/kernel.h>
#include <sync/spinlock.h>

typedef struct {
    black_spinlock_t black_lock;
    int black_locked;
    uint32_t black_owner;
} black_mutex_t;

void black_mutex_init(black_mutex_t *black_m) {
    black_m->black_lock.black_lock = 0;
    black_m->black_lock.black_ticket = 0;
    black_m->black_lock.black_serving = 0;
    black_m->black_locked = 0;
    black_m->black_owner = 0;
}

void black_mutex_lock(black_mutex_t *black_m) {
    while (__sync_lock_test_and_set(&black_m->black_locked, 1)) {
        __asm__ volatile("pause");
    }
}

void black_mutex_unlock(black_mutex_t *black_m) {
    __sync_lock_release(&black_m->black_locked);
}
