#ifndef _BLACK_SIGNAL_H
#define _BLACK_SIGNAL_H
#include <stdint.h>
void black_signal_init(void);
int black_signal_send(uint32_t black_pid, int black_signum);
void black_signal_handle(int black_signum);
#endif
