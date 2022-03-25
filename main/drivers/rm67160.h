
/**
 * @file rm67160.h
 */

#pragma once

#include "render_common.h"

void rm67160_config(screen_driver_t *driver, screen_driver_param_t *param);

void rm67160_init(screen_driver_t *driver);
void rm67160_fill_rect(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void rm67160_fill_rect_async(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool rm67160_fill_ready(screen_driver_t *driver);
void rm67160_release(screen_driver_t *driver);
void rm67160_fill(screen_driver_t *driver, const void* data);
void rm67160_uninit(screen_driver_t *driver);
