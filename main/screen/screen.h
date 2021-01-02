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

void screen_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t data_size);
void screen_test(void);