/**
 * @file gc9107.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "render_common.h"


void gc9107_config(screen_driver_t *driver, screen_driver_param_t *param);

uint8_t gc9107_type(screen_driver_t *driver);
void gc9107_init(screen_driver_t *driver);
void gc9107_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void gc9107_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool gc9107_fill_ready(screen_driver_t *driver);
void gc9107_release(screen_driver_t *driver);
void gc9107_fill(screen_driver_t *driver, const void* data);
void gc9107_uninit(screen_driver_t *driver);

