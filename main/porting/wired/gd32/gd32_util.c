/**
 * @file gd32_util.c
 * 
 */

#include "generic_hal.h"
#include "gd32_util.h"

#ifndef SYSTICK_FREQ
#define SYSTICK_FREQ    1
#endif

volatile static uint32_t systick_counter = 0;

// always 1K HZ
void systick_init(void)
{
    SysTick_Config(SystemCoreClock / (1000/SYSTICK_FREQ));
    //nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    //nvic_irq_enable((uint8_t)SysTick_IRQn, 3U, 0U);
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void systick_delay(uint32_t ticks)
{
    //usb_delay_ms(ticks);
    //return;

    uint32_t tick_start = systick_get_tick();
    uint32_t wait = ticks;

    /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFUL)
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

void SysTick_Handler(void)
{
    systick_inc_tick();
}

void usb_delay_ms(uint32_t ms)
{
    //systick_delay(ms);
    usb_delay_us(ms*1000);
}

// approximate us delay
void usb_delay_us(uint32_t us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
}
