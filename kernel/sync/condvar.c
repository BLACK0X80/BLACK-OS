#include <kernel/kernel.h>
#include <sync/spinlock.h>

void black_condvar_init(black_spinlock_t *black_lock) {
    (void)black_lock;
}

void black_condvar_wait(black_spinlock_t *black_lock) {
    black_spinlock_release(black_lock);
    __asm__ volatile("pause");
    black_spinlock_acquire(black_lock);
}

void black_condvar_signal(black_spinlock_t *black_lock) {
    (void)black_lock;
}
