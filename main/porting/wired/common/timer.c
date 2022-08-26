/**
 * timer.c
 * timer based on systemtick
 */

#include "generic_hal.h"
#include "timer.h"

#ifdef RTOS_ENABLE
#include "os_tick.h"
#endif

extern uint32_t systick_get_tick(void);

void timer_init(void) {}

void timer_clear(void) {}

uint16_t timer_read(void)
{
    return (uint16_t)timer_read32();
}

uint32_t timer_read32(void)
{
#ifdef RTOS_ENABLE
    return OS_Tick_GetCount();
#else
    return systick_get_tick();
#endif
}

uint16_t timer_elapsed(uint16_t last)
{
    return TIMER_DIFF_16(timer_read(), last);
}

uint32_t timer_elapsed32(uint32_t last)
{
    return TIMER_DIFF_32(timer_read32(), last);
}