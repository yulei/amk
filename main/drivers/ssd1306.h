/**
 * @file ssd1306.h
 * @author astro
 * @brief 
 * 
 * @copyright Copyright (c) 2022, astro,  All rights reserved.
 * 
 */

#pragma once

#include "render_common.h"


void ssd1306_config(screen_driver_t *driver, screen_driver_param_t *param);

uint8_t ssd1306_type(screen_driver_t *driver);
void ssd1306_init(screen_driver_t *driver);
void ssd1306_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void ssd1306_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool ssd1306_fill_ready(screen_driver_t *driver);
void ssd1306_release(screen_driver_t *driver);
void ssd1306_fill(screen_driver_t *driver, const void* data);
void ssd1306_uninit(screen_driver_t *driver);

