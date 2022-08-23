/**
 * @file amk_timer.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "timer.h"

#define TIMER_DIFF_FAST(a, b) TIMER_DIFF_32(a, b)
#define timer_expired_fast(current, future) timer_expired32(current, future)
typedef uint32_t fast_timer_t;
fast_timer_t inline timer_read_fast(void)
{
    return timer_read32();
}

fast_timer_t inline timer_elapsed_fast(fast_timer_t last)
{
    return timer_elapsed32(last);
}