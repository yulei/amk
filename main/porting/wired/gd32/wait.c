/**
 * @file wait.c
 */

#include "generic_hal.h"
#include "gd32_util.h"
#include "wait.h"
//#include "cmsis_os2.h"

void wait_ms(int ms)
{
#ifdef RTOS_ENABLE
    //osDelay(ms);
    usb_delay_ms(ms);
#else
    systick_delay(ms);
#endif
}

void wait_us(int us)
{
#ifdef DWT_DELAY
    __IO uint32_t startTick = DWT->CYCCNT;
    __IO uint32_t delayTicks = us * (SystemCoreClock/1000000);

    while (DWT->CYCCNT - startTick < delayTicks);
#else
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
#endif
}

