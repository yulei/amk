/**
 * @file bl5372.c
 */

#include "bl5372.h"
#include "i2c.h"
#include "amk_printf.h"

#ifndef BL5372_DEBUG
#define BL5372_DEBUG 1
#endif

#if BL5372_DEBUG 
#define bl5372_debug  amk_printf
#else
#define bl5372_debug(...)
#endif

#define SECOND_REG              0x00
#define MINUTE_REG              0x01
#define HOUR_REG                0x02
#define DOW_REG                 0x03
#define DAY_REG                 0x04
#define MONTH_REG               0x05
#define YEAR_REG                0x06
#define ADJUST_REG              0x07

#define CTRL1_REG               0x0E
#define CTRL2_REG               0x0F

#define TIMEOUT                 100

#ifndef BL5372_I2C_ID
#define BL5372_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;

void rtc_driver_init(void)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(BL5372_I2C_ID);
    }
}

void rtc_driver_write(const rtc_datetime_t *datetime)
{
    uint8_t buf[7];
    buf[0] = dec2bcd(datetime->second);
    buf[1] = dec2bcd(datetime->minute);
    buf[2] = dec2bcd(datetime->hour);
    buf[3] = dayofweek(datetime->day, datetime->month, datetime->year+2000);
    buf[4] = dec2bcd(datetime->day);
    buf[5] = dec2bcd(datetime->month);
    buf[6] = dec2bcd(datetime->year);

    amk_error_t error = i2c_write_reg(i2c_inst, BL5372_ADDR, SECOND_REG, buf, 7, TIMEOUT);
    if (error != AMK_SUCCESS) {
        bl5372_debug("BL5372: failed to write time:%d\n", error);
    }
}

void rtc_driver_read(rtc_datetime_t *datetime)
{
    uint8_t buf[7];
    amk_error_t error = i2c_read_reg(i2c_inst, BL5372_ADDR, SECOND_REG, buf, 7, TIMEOUT);
    if (error != AMK_SUCCESS) {
        bl5372_debug("BL5372: failed to read time:%d\n", error);
        return;
    }

    datetime->second = bcd2dec(buf[0]);
    datetime->minute = bcd2dec(buf[1]);
    datetime->hour   = bcd2dec(buf[2]);
    datetime->day    = bcd2dec(buf[4]);
    datetime->month  = bcd2dec(buf[5]);
    datetime->year   = bcd2dec(buf[6]);
}
