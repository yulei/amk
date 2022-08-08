
/**
 * @file st7789.h
 */

#pragma once

#include "render_def.h"

void st7789_config(screen_driver_t *driver, screen_driver_param_t *param);

uint8_t st7789_type(screen_driver_t *driver);
void st7789_init(screen_driver_t *driver);
void st7789_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void st7789_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool st7789_fill_ready(screen_driver_t *driver);
void st7789_release(screen_driver_t *driver);
void st7789_fill(screen_driver_t *driver, const void* data);
void st7789_uninit(screen_driver_t *driver);
