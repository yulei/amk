/**
 * wait.c
 */

#include "generic_hal.h"
#include "wait.h"

#ifdef RTOS_ENABLE
#include "tx_api.h"
void wait_ms(int ms)
{
    tx_thread_sleep((ms*TX_TIMER_TICKS_PER_SECOND)/1000);
}
#else
void wait_ms(int ms)
{
    HAL_Delay(ms);
}
#endif

#ifdef DWT_DELAY
void wait_us(int us)
{
    __IO int32_t ticks = DWT->CYCCNT + us * (SystemCoreClock/1000000);
    while (DWT->CYCCNT <= ticks);
}
#else
void wait_us(int us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
}
#endif