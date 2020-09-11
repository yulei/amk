/**
 * screen.h
 */

#pragma once

#include <stdint.h>

void screen_init(void);
void screen_task(void);
void screen_ticks(uint32_t ticks);
void screen_uninit(void);