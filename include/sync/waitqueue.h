#ifndef _BLACK_WAITQUEUE_H
#define _BLACK_WAITQUEUE_H
typedef struct { volatile int black_count; } black_waitqueue_t;
void black_waitqueue_init(black_waitqueue_t *black_wq);
void black_waitqueue_sleep(black_waitqueue_t *black_wq);
void black_waitqueue_wake(black_waitqueue_t *black_wq);
#endif
