/**
 * wait.c
 */

#include "generic_hal.h"
#include "wait.h"

void wait_ms(int ms)
{
    HAL_Delay(ms);
}

void wait_us(int us)
{
//#ifdef STM32L072xx
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
//#else
//    __IO int32_t ticks = DWT->CYCCNT + us * (SystemCoreClock/1000000);
//    while (DWT->CYCCNT <= ticks);
//#endif
}