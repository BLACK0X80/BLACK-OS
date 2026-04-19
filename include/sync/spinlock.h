#ifndef _BLACK_SPINLOCK_H
#define _BLACK_SPINLOCK_H

#include <stdint.h>

typedef struct {
    volatile uint32_t black_lock;
    volatile uint32_t black_ticket;
    volatile uint32_t black_serving;
} black_spinlock_t;

#define SPINLOCK_INIT { 0, 0, 0 }

void black_spinlock_acquire(black_spinlock_t *black_lock);
void black_spinlock_release(black_spinlock_t *black_lock);
void black_spinlock_irq_save(black_spinlock_t *black_lock, uint32_t *black_flags);
void black_spinlock_irq_restore(black_spinlock_t *black_lock, uint32_t black_flags);

#define spinlock_acquire     black_spinlock_acquire
#define spinlock_release     black_spinlock_release
#define spinlock_irq_save    black_spinlock_irq_save
#define spinlock_irq_restore black_spinlock_irq_restore

typedef black_spinlock_t spinlock_t;

#endif
