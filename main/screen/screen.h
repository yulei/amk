/**
 * @file screen.h
 * 
 */

#pragma once

#include "render_def.h"

screen_t* screen_create(screen_param_t *param);
void screen_destroy(screen_t *screen);
