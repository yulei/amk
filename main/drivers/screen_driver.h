/**
 * @file screen_driver.h
 * 
 */

#pragma once

#include "render_common.h"

screen_driver_t* screen_driver_create(screen_driver_param_t *param);
void screen_driver_destroy(screen_driver_t *driver);
