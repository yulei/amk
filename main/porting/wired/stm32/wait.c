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

void wait_us(int us)
{
#ifdef STM32L072xx
    extern TIM_HandleTypeDef WAIT_TIMER_ID;
    __HAL_TIM_SET_COUNTER(&WAIT_TIMER_ID,0);
	while (__HAL_TIM_GET_COUNTER(&WAIT_TIMER_ID) < us);
#else
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
#endif
//#else
//    __IO int32_t ticks = DWT->CYCCNT + us * (SystemCoreClock/1000000);
//    while (DWT->CYCCNT <= ticks);
//#endif
}