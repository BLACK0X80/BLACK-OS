#ifndef _BLACK_SEMAPHORE_H
#define _BLACK_SEMAPHORE_H
typedef struct { volatile int black_count; } black_semaphore_t;
void black_semaphore_init(black_semaphore_t *black_sem, int black_val);
void black_semaphore_wait(black_semaphore_t *black_sem);
void black_semaphore_signal(black_semaphore_t *black_sem);
#endif
