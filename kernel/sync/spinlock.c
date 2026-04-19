#include <sync/spinlock.h>
#include <kernel/kernel.h>

void black_spinlock_acquire(black_spinlock_t *black_lock)
{
    uint32_t black_my_ticket = __sync_fetch_and_add(&black_lock->black_ticket, 1);
    while (__sync_val_compare_and_swap(&black_lock->black_serving, black_my_ticket, black_my_ticket) != black_my_ticket) {
        __asm__ volatile("pause");
    }
}

void black_spinlock_release(black_spinlock_t *black_lock)
{
    __sync_fetch_and_add(&black_lock->black_serving, 1);
}

void black_spinlock_irq_save(black_spinlock_t *black_lock, uint32_t *black_flags)
{
    __asm__ volatile("pushf; pop %0; cli" : "=r"(*black_flags));
    black_spinlock_acquire(black_lock);
}

void black_spinlock_irq_restore(black_spinlock_t *black_lock, uint32_t black_flags)
{
    black_spinlock_release(black_lock);
    __asm__ volatile("push %0; popf" : : "r"(black_flags));
}
