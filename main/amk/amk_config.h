/**
 * common_config.h
 *  common config fot keymap 
 */

#pragma once

#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"
#include "actionmap.h"
#include "keymap.h"
#include "amk_gpio.h"
#include "amk_action.h"
#include "amk_printf.h"

#ifdef RGB_ENABLE
#include "rgb_effects.h"
#endif

#ifdef RGB_MATRIX_ENABLE
#include "rgb_matrix.h"
#endif