/**
 * onekey.h
 */

#pragma once

#include "config.h"
#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"
#include "keymap.h"

#define LAYOUT_default(k00) { {k00} }

#define LAYOUT_ext(k00, k01, k02, k10, k11, k12) { {k00, k01, k02}, {k10, k11, k12} }