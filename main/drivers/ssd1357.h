/**
 * @file ssd1357.h
 */

#pragma once

#include "render_def.h"


void ssd1357_config(screen_driver_t *driver, screen_driver_param_t *param);

uint8_t ssd1357_type(screen_driver_t *driver);
void ssd1357_init(screen_driver_t *driver);
void ssd1357_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void ssd1357_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool ssd1357_fill_ready(screen_driver_t *driver);
void ssd1357_release(screen_driver_t *driver);
void ssd1357_fill(screen_driver_t *driver, const void* data);
void ssd1357_uninit(screen_driver_t *driver);
