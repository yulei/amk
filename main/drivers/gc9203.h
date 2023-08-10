/**
 * @file gc9203.h
 * @author astro
 * 
 * @copyright Copyright (C) 2023
*/

#pragma once

#include "render_def.h"


void gc9203_config(screen_driver_t *driver, screen_driver_param_t *param);

uint8_t gc9203_type(screen_driver_t *driver);
void gc9203_init(screen_driver_t *driver);
void gc9203_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void gc9203_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool gc9203_fill_ready(screen_driver_t *driver);
void gc9203_release(screen_driver_t *driver);
void gc9203_fill(screen_driver_t *driver, const void* data);
void gc9203_uninit(screen_driver_t *driver);

