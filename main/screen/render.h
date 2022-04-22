/**
 * @file render.h
 * 
 */

#pragma once

#include "render_common.h"

extern screen_driver_param_t screen_driver_params[SCREEN_DRIVER_NUM];
extern screen_param_t screen_params[SCREEN_NUM];
extern display_param_t display_params[DISPLAY_NUM];

void render_init(void);

void render_task(void);

void render_toggle_display(uint8_t display);

void render_enable_display(uint8_t display, bool enable);