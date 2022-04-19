/**
 * @file gd32_util.h
 * 
 */

#pragma once

#include <stdint.h>

void systick_init(void);
void systick_delay(uint32_t ticks);
void systick_inc_tick(void);
uint32_t systick_get_tick(void);
void systick_suspend(void);
void systick_resume(void);

void usb_delay_ms(uint32_t ms);
void usb_delay_us(uint32_t us);
