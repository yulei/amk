/**
 * @file screen.h
 * 
 */

#pragma once

#include "render_common.h"

screen_t* screen_create(screen_param_t *param);
void screen_destroy(screen_t *screen);
