/**
 * wait.c
 */

#include "generic_hal.h"
#include "wait.h"

void wait_ms(int ms)
{
    delay_ms(ms);
}

void wait_us(int us)
{
    delay_us(us);
}