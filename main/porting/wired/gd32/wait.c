/**
 * @file wait.c
 */

#include "generic_hal.h"
#include "gd32_util.h"
#include "wait.h"

void wait_ms(int ms)
{
    systick_delay(ms);
}

void wait_us(int us)
{
    __IO uint32_t startTick = DWT->CYCCNT;
    __IO uint32_t delayTicks = us * (SystemCoreClock/1000000);

    while (DWT->CYCCNT - startTick < delayTicks);
}
