#ifndef _BLACK_ATOMIC_H
#define _BLACK_ATOMIC_H
#include <stdint.h>
static inline int32_t black_atomic_add(volatile int32_t *black_ptr, int32_t black_val) {
    return __sync_add_and_fetch(black_ptr, black_val);
}
static inline int32_t black_atomic_sub(volatile int32_t *black_ptr, int32_t black_val) {
    return __sync_sub_and_fetch(black_ptr, black_val);
}
static inline int32_t black_atomic_cas(volatile int32_t *black_ptr, int32_t black_old, int32_t black_new) {
    return __sync_val_compare_and_swap(black_ptr, black_old, black_new);
}
#endif
