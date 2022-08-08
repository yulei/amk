/**
 * @file display.h
 * 
 */

#pragma once

#include "render_def.h"

display_t* display_create(display_param_t *param);
void display_destroy(display_t *screen);
