/**
 * wait.c
 *  busy delay
 */

#include "wait.h"
#include "nrf_delay.h"

void wait_ms(int ms)
{
    nrf_delay_ms(ms);
}

void wait_us(int us)
{
    nrf_delay_us(us);
}