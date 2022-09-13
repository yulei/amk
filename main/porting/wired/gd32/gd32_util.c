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
#if 0
// using timer 2 as system tick source
void systick_init(void)
{
    timer_parameter_struct timer_initpara;

    NVIC_SetPriority(TIMER2_IRQn, 0x01U);

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER2);

    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 167;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    gd32_timer_init(TIMER2, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER2);
}

void systick_delay(uint32_t ticks)
{
    uint32_t tick_start = systick_get_tick();
    uint32_t wait = ticks;

    /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFUL)
    {
        wait ++;
    }

    while((systick_get_tick() - tick_start) < wait) {
        ;
    }
}

void systick_inc_tick(void)
{
    systick_counter ++;
}

uint32_t systick_get_tick(void)
{
    return systick_counter;
}

void systick_suspend(void)
{
    timer_interrupt_disable(TIMER2, TIMER_INT_UP);
}

void systick_resume(void)
{
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
}

void TIMER2_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_UP) != RESET){
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);
        systick_inc_tick();
    }
}

#else
// always 1K HZ
void systick_init(void)
{
    SysTick_Config(SystemCoreClock / (1000/SYSTICK_FREQ));
    //nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    //nvic_irq_enable((uint8_t)SysTick_IRQn, 3U, 0U);
    NVIC_SetPriority(SysTick_IRQn, 0x0FU);
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

#ifndef RTOS_ENABLE
void SysTick_Handler(void)
{
    systick_inc_tick();
}
#endif
#endif

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
