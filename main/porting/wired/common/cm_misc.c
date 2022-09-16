/**
 * @file cm_misc.c
 * 
 */

#include "generic_hal.h"
#include "cm_misc.h"
#include "wait.h"

#ifndef SYSTICK_FREQ
#define SYSTICK_FREQ    1
#endif

volatile static uint32_t systick_counter = 0;

// always 1K HZ
void systick_init(void)
{
    SysTick_Config(SystemCoreClock / (1000/SYSTICK_FREQ));
    NVIC_SetPriority(SysTick_IRQn, 0x0FU);
}

void systick_delay(uint32_t ticks)
{
    uint32_t tick_start = systick_get_tick();
    uint32_t wait = ticks;

    /* Add a freq to guarantee minimum wait */
    if (!wait)
    {
        wait += SYSTICK_FREQ;
    }

    while((systick_get_tick() - tick_start) < wait) {
        ;
    }
}

void systick_inc_tick(void)
{
    systick_counter += SYSTICK_FREQ;
}

uint32_t systick_get_tick(void)
{
    return systick_counter;
}

void systick_suspend(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

 void systick_resume(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

#ifndef RTOS_ENABLE
#ifdef HC32F460
void SysTick_IrqHandler(void)
#else
void SysTick_Handler(void)
#endif
{
    systick_inc_tick();
#ifdef HC32F460
    systick_inc_tick();
#endif
}
#endif

void busy_delay_ms(uint32_t ms)
{
    busy_delay_us(ms*1000);
}

// approximate us delay
void busy_delay_us(uint32_t us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
}

#ifdef RTOS_ENABLE
#include "cmsis_os2.h"
#endif

void wait_ms(int ms)
{
#ifdef RTOS_ENABLE
    if (osKernelGetState() == osKernelRunning) {
        osDelay(ms);
    } else {
        busy_delay_ms(ms);
    }
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
    busy_delay_us(us);
#endif
}

void dwt_delay_init(void)
{
#ifdef DWT_DELAY
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif
}
