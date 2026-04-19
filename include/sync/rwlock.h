#ifndef _BLACK_RWLOCK_H
#define _BLACK_RWLOCK_H
typedef struct { volatile int black_readers; volatile int black_writer; } black_rwlock_t;
void black_rwlock_init(black_rwlock_t *black_rw);
void black_rwlock_read_lock(black_rwlock_t *black_rw);
void black_rwlock_read_unlock(black_rwlock_t *black_rw);
void black_rwlock_write_lock(black_rwlock_t *black_rw);
void black_rwlock_write_unlock(black_rwlock_t *black_rw);
#endif
