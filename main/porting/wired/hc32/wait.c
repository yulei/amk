/**
 * @file wait.c
 */

#include "generic_hal.h"
#include "wait.h"

void wait_ms(int ms)
{
    SysTick_Delay(ms);
}

#ifdef DWT_DELAY
void wait_us(int us)
{
    __IO uint32_t startTick = DWT->CYCCNT;
    __IO uint32_t delayTicks = us * (SystemCoreClock/1000000);

    while (DWT->CYCCNT - startTick < delayTicks);
}
#else
void wait_us(int us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
}
#endif
