#ifndef _BLACK_RTC_H
#define _BLACK_RTC_H

#include <stdint.h>

typedef struct {
    uint8_t  black_second;
    uint8_t  black_minute;
    uint8_t  black_hour;
    uint8_t  black_day;
    uint8_t  black_month;
    uint16_t black_year;
    uint8_t  black_weekday;
} black_rtc_time_t;

void black_rtc_init(void);
void black_rtc_get_time(black_rtc_time_t *black_time);
uint32_t black_rtc_get_unix_timestamp(void);

#define rtc_init           black_rtc_init
#define rtc_get_time       black_rtc_get_time
#define rtc_get_unix_timestamp black_rtc_get_unix_timestamp

#endif
