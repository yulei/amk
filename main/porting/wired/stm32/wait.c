/**
 * wait.c
 */

#include "generic_hal.h"
#include "wait.h"

#ifdef RTOS_ENABLE
#include "cmsis_os2.h"
void wait_ms(int ms)
{
    HAL_Delay(ms);
    //osDelay(ms);
    //tx_thread_sleep((ms*1000)/TX_TIMER_TICKS_PER_SECOND);
}
#else
void wait_ms(int ms)
{
    HAL_Delay(ms);
}
#endif

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