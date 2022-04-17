/**
 * @file wait.c
 */

#include "generic_hal.h"
#include "wait.h"

void wait_ms(int ms)
{
    SysTick_Delay(ms);
}

void wait_us(int us)
{
    __IO int32_t ticks = DWT->CYCCNT + us * (SystemCoreClock/1000000);
    while (DWT->CYCCNT <= ticks);
}
