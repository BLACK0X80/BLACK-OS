#include <drivers/rtc.h>
#include <kernel/kernel.h>

static uint8_t black_rtc_read(uint8_t black_reg)
{
    black_outb(0x70, black_reg);
    return black_inb(0x71);
}

static uint8_t black_bcd_to_bin(uint8_t black_bcd)
{
    return ((black_bcd >> 4) * 10) + (black_bcd & 0x0F);
}

static int black_rtc_is_updating(void)
{
    black_outb(0x70, 0x0A);
    return black_inb(0x71) & 0x80;
}

void black_rtc_init(void) {}

void black_rtc_get_time(black_rtc_time_t *black_t)
{
    while (black_rtc_is_updating());
    black_t->black_second = black_bcd_to_bin(black_rtc_read(0x00));
    black_t->black_minute = black_bcd_to_bin(black_rtc_read(0x02));
    black_t->black_hour   = black_bcd_to_bin(black_rtc_read(0x04));
    black_t->black_day    = black_bcd_to_bin(black_rtc_read(0x07));
    black_t->black_month  = black_bcd_to_bin(black_rtc_read(0x08));
    black_t->black_year   = black_bcd_to_bin(black_rtc_read(0x09)) + 2000;
    black_t->black_weekday = black_bcd_to_bin(black_rtc_read(0x06));
}

uint32_t black_rtc_get_unix_timestamp(void)
{
    black_rtc_time_t black_t;
    black_rtc_get_time(&black_t);
    uint32_t black_days = 0;
    for (uint16_t black_y = 1970; black_y < black_t.black_year; black_y++) {
        black_days += (black_y % 4 == 0 && (black_y % 100 != 0 || black_y % 400 == 0)) ? 366 : 365;
    }
    static const uint16_t black_mdays[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
    if (black_t.black_month > 0 && black_t.black_month <= 12)
        black_days += black_mdays[black_t.black_month - 1];
    if (black_t.black_month > 2 && (black_t.black_year % 4 == 0 && (black_t.black_year % 100 != 0 || black_t.black_year % 400 == 0)))
        black_days++;
    black_days += black_t.black_day - 1;
    return black_days * 86400 + black_t.black_hour * 3600 + black_t.black_minute * 60 + black_t.black_second;
}
