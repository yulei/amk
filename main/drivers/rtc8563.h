/**
 * @file rtc8563.h
 */

#pragma once

#include <stdint.h>

#ifndef RTC8563_ADDR
    #define RTC8563_ADDR 0xA2
#endif

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} rtc_datetime_t;

void rtc8563_init(void);
void rtc8563_write_time(const rtc_datetime_t *datetime);
void rtc8563_read_time(rtc_datetime_t *datetime);
