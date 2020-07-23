/**
 * timer.c
 *  timer implementation on nrf52
 */

#include "timer.h"
#include "app_timer.h"

/* ticks per ms*/
#define TICKS_PER_MS (APP_TIMER_TICKS(1))

void timer_init(void) {}

void timer_clear(void) {}

uint16_t timer_read(void)
{
  return (uint16_t)(timer_read32());
}

uint32_t timer_read32(void)
{
  return (app_timer_cnt_get()/TICKS_PER_MS);
}

uint16_t timer_elapsed(uint16_t last)
{
  return (uint16_t)(timer_elapsed32(last));
}

uint32_t timer_elapsed32(uint32_t last)
{
  uint32_t ticks = app_timer_cnt_diff_compute(app_timer_cnt_get(), APP_TIMER_TICKS(last));
  return (ticks / TICKS_PER_MS);
}
