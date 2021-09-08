/**
 * @file rtc_driver.h
 */

#pragma once

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} rtc_datetime_t;

static inline uint8_t bcd2dec(uint8_t bcd)
{
    return (((bcd & 0xF0)>>4) *10) + (bcd & 0xF);
}

static inline uint8_t dec2bcd(uint8_t dec)
{
	return ((dec / 10)<<4) + (dec % 10);
}

static inline uint8_t dayofweek(int day, int month, int year)
{
    int weekday  = (day += month < 3 ? year-- : year - 2, 23*month/9 + day + 4 + year/4- year/100 + year/400) % 7;
    return (uint8_t)weekday;
}

void rtc_driver_init(void);
void rtc_driver_write(const rtc_datetime_t *datetime);
void rtc_driver_read(rtc_datetime_t *datetime);