/**
 * @file info_display.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "render_def.h"

typedef struct {
    uint8_t led;
    uint8_t mods;
    uint8_t layer;
} info_status_t;

extern info_status_t info_status;

bool info_display_create(display_t *display, display_param_t *param);