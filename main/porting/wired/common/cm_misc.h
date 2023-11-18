/**
 * @file cm_misc.h
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

void busy_delay_ms(uint32_t ms);
void busy_delay_us(uint32_t us);

void dwt_delay_init(void);

uint32_t magic_read(void);
void magic_write(uint32_t magic);

#define RESET_MSC   (1<<0)      // reset to msc
#define RESET_HS    (1<<1)      // reset to high speed
#define RESET_FS    (1<<2)      // reset to full speed

uint32_t reset_read(void);
void reset_write(uint32_t reason);
