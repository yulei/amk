/**
 * timer.c
 * timer based on systemtick
 */

#include "stm32f4xx_hal.h"
#include "timer.h"

void timer_init(void) {}

void timer_clear(void) {}

uint16_t timer_read(void)
{
    return (uint16_t)timer_read32();
}

uint32_t timer_read32(void)
{
    return HAL_GetTick();
}

uint16_t timer_elapsed(uint16_t last)
{
    return TIMER_DIFF_16(timer_read(), last);
}

uint32_t timer_elapsed32(uint32_t last)
{
    return TIMER_DIFF_32(timer_read32(), last);
}