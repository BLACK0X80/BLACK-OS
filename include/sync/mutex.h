#ifndef _BLACK_MUTEX_H
#define _BLACK_MUTEX_H
#include <sync/spinlock.h>
typedef struct { black_spinlock_t black_lock; int black_locked; uint32_t black_owner; } black_mutex_t;
void black_mutex_init(black_mutex_t *black_m);
void black_mutex_lock(black_mutex_t *black_m);
void black_mutex_unlock(black_mutex_t *black_m);
#endif
