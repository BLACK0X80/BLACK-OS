#ifndef _BLACK_CONDVAR_H
#define _BLACK_CONDVAR_H
#include <sync/spinlock.h>
void black_condvar_init(black_spinlock_t *black_lock);
void black_condvar_wait(black_spinlock_t *black_lock);
void black_condvar_signal(black_spinlock_t *black_lock);
#endif
