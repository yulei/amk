/**
 * wait.c
 */

#include "wait.h"
#include "drv_usb_hw.h"

void wait_ms(int ms)
{
    usb_mdelay(ms);
}

void wait_us(int us)
{
    usb_udelay(us);
}