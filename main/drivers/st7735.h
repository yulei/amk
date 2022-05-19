
/**
 * @file st7735.h
 */

#pragma once

#include "render_common.h"


void st7735_config(screen_driver_t *driver, screen_driver_param_t *param);

uint8_t st7735_type(screen_driver_t *driver);
void st7735_init(screen_driver_t *driver);
void st7735_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void st7735_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool st7735_fill_ready(screen_driver_t *driver);
void st7735_release(screen_driver_t *driver);
void st7735_fill(screen_driver_t *driver, const void* data);
void st7735_uninit(screen_driver_t *driver);

