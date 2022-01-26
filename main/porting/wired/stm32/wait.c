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
    //extern TIM_HandleTypeDef WAIT_TIMER_ID;
    //__HAL_TIM_SET_COUNTER(&WAIT_TIMER_ID,0);
	//while (__HAL_TIM_GET_COUNTER(&WAIT_TIMER_ID) < us);
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
#else
    #if 1
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
    #else
    __IO uint32_t start = DWT->CYCCNT;
    __IO uint32_t delay = us * (SystemCoreClock/1000000);
    while (DWT->CYCCNT - start < delay);
    #endif
#endif
}

#ifndef STM32L072xx
void dwt_wait_us(int us)
{
    __IO uint32_t start = DWT->CYCCNT;
    __IO uint32_t delay = us * (SystemCoreClock/1000000);
    while (DWT->CYCCNT - start < delay);
}
#endif