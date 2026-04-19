#ifndef _BLACK_PIT_H
#define _BLACK_PIT_H

#include <stdint.h>

void     black_pit_init(uint32_t black_freq);
uint64_t black_pit_get_ticks(void);
uint64_t black_pit_get_uptime_ms(void);
void     black_pit_sleep(uint32_t black_ms);
void     black_pit_use_lapic_timer(void);

#define pit_init          black_pit_init
#define pit_get_ticks     black_pit_get_ticks
#define pit_get_uptime_ms black_pit_get_uptime_ms
#define pit_sleep         black_pit_sleep
#define pit_use_lapic_timer black_pit_use_lapic_timer

#endif
