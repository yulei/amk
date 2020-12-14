/**
 * screen.h
 */

#pragma once

#include <stdint.h>

void screen_init(void);
void screen_task(void);
void screen_ticks(uint32_t ticks);
void screen_uninit(void);
void screen_fill(const void* data);

void screen_fill_rect(void* data);
void screen_test(void);