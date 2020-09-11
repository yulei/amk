/**
 * lvgl_driver.h
 */

#pragma once

#include <stdint.h>

void lvgl_driver_init(void);
void lvgl_driver_task(void);
void lvgl_driver_uninit(void);
void lvgl_driver_ticks(uint32_t ticks);