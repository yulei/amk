/**
 * @file rtc8563.c
 */

#include "rtc8563.h"
#include "i2c.h"
#include "amk_printf.h"

#ifndef RTC8563_DEBUG
#define RTC8563_DEBUG 1
#endif

#if RTC8563_DEBUG 
#define rtc8563_debug  amk_printf
#else
#define rtc8563_debug(...)
#endif

#define CONTROL1_REGISTER       0x00
#define SECONDS_REGISTER        0x02
#define CLOCK_REGISTER          0x0D

#define TIMEOUT                 100

#ifndef RTC8563_I2C_ID
#define RTC8563_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;

void rtc_driver_init(void)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(RTC8563_I2C_ID);
    }
    amk_error_t error = AMK_SUCCESS;
    uint8_t d = 0; 
    error = i2c_read_reg(i2c_inst, RTC8563_ADDR, CLOCK_REGISTER, &d, 1, TIMEOUT);
    rtc8563_debug("rtc8563: read clock register=%d, error=%d\n", d, error);
    d |= 0x03;
    error = i2c_write_reg(i2c_inst, RTC8563_ADDR, CLOCK_REGISTER, &d, 1, TIMEOUT);
    rtc8563_debug("rtc8563: write clock register=%d, error=%d\n", d, error);

    d = 0;
    error = i2c_read_reg(i2c_inst, RTC8563_ADDR, CONTROL1_REGISTER, &d, 1, TIMEOUT);
    rtc8563_debug("rtc8563: read control 1 register=%d, error=%d\n", d, error);
    d &= ~(1<<5);
    i2c_write_reg(i2c_inst, RTC8563_ADDR, CONTROL1_REGISTER, &d, 1, TIMEOUT);
    rtc8563_debug("rtc8563: write control 1 register=%d, error=%d\n", d, error);
}

void rtc_driver_write(const rtc_datetime_t *datetime)
{
    uint8_t buf[7];
    buf[0] = dec2bcd(datetime->second);
    buf[1] = dec2bcd(datetime->minute);
    buf[2] = dec2bcd(datetime->hour);
    buf[3] = dec2bcd(datetime->day);
    buf[4] = dayofweek(datetime->day, datetime->month, datetime->year+2000);
    buf[5] = dec2bcd(datetime->month);
    buf[6] = dec2bcd(datetime->year);

    amk_error_t error = i2c_write_reg(i2c_inst, RTC8563_ADDR, SECONDS_REGISTER, buf, 7, TIMEOUT);
    if (error != AMK_SUCCESS) {
        rtc8563_debug("RTC8563: failed to write time:%d\n", error);
    }
}

void rtc_driver_read(rtc_datetime_t *datetime)
{
    uint8_t buf[7];
    amk_error_t error = i2c_read_reg(i2c_inst, RTC8563_ADDR, SECONDS_REGISTER, buf, 7, TIMEOUT);
    if (error != AMK_SUCCESS) {
        rtc8563_debug("RTC8563: failed to read time:%d\n", error);
        return;
    }

    datetime->second = bcd2dec(buf[0]);
    datetime->minute = bcd2dec(buf[1]);
    datetime->hour   = bcd2dec(buf[2]);
    datetime->day    = bcd2dec(buf[3]);
    datetime->month  = bcd2dec(buf[5]);
    datetime->year   = bcd2dec(buf[6]);
}
