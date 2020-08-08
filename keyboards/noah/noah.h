/**
 * noah.h
 */

#pragma once

#include "config.h"
#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"
#include "keymap.h"

#define LAYOUT_hotswap( \
    k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0a, k0b, k0c, k0d, k0e, k0f, \
	k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1a, k1b, k1c, k1d, k1e, \
	k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b,      k2d, k2e, \
	k30,      k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d, k3e, \
	k40,      k42, k43,      k45, k46, k47, k48 \
) { \
    {k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0a, k0b, k0c, k0d, k0e}, \
    {k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1a, k1b, k1c, k1d, KC_NO}, \
    {k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b,KC_NO,k2d, KC_NO}, \
    {k30,KC_NO,k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d, KC_NO}, \
    {k40,KC_NO,k42,KC_NO,k43,KC_NO,KC_NO, k45, k46, k47, k48, k1e, k2e, k3e, k0f}  \
}
