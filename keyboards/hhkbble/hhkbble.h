/**
  * hhkbble.h
  *
  */
#pragma once

#include "config.h"
#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"
#include "keymap.h"
#include "custom_action.h"

#define LAYOUT_default( \
    k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0a, k0b, k0c, k0d, k0e, \
	k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1a, k1b, k1c, k1d, \
	k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b,      k2d, \
	k30,      k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d, \
	k41, k42, k43, k44, k45) \
{ \
    {  k00,  k01,k02,  k03,  k04,  k05,k06,  k07,  k08,  k09,  k0a,k0b,  k0c,k0d}, \
    {  k10,  k11,k12,  k13,  k14,  k15,k16,  k17,  k18,  k19,  k1a,k1b,  k1c,k1d}, \
    {  k20,  k21,k22,  k23,  k24,  k25,k26,  k27,  k28,  k29,  k2a,k2b,KC_NO,k2d}, \
    {  k30,KC_NO,k32,  k33,  k34,  k35,k36,  k37,  k38,  k39,  k3a,k3b,  k3c,k3d}, \
    {KC_NO,  k41,k42,KC_NO,KC_NO,KC_NO,KC_NO,k43,KC_NO,KC_NO,KC_NO,k44,  k45,k0e}  \
}

#define LAYOUT_iso( \
    k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0a, k0b, k0c, k0d, \
	k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1a, k1b, k1c, \
	k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b, k2c, k2d, \
	k30, k31, k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d, \
	k41, k42, k43, k44, k45) \
{ \
    {  k00,  k01,k02,  k03,  k04,  k05,k06,  k07,  k08,  k09,  k0a,k0b,  k0c,  k0d}, \
    {  k10,  k11,k12,  k13,  k14,  k15,k16,  k17,  k18,  k19,  k1a,k1b,  k1c,KC_NO}, \
    {  k20,  k21,k22,  k23,  k24,  k25,k26,  k27,  k28,  k29,  k2a,k2b,  k2c,  k2d}, \
    {  k30,  k31,k32,  k33,  k34,  k35,k36,  k37,  k38,  k39,  k3a,k3b,  k3c,  k3d}, \
    {KC_NO,  k41,k42,KC_NO,KC_NO,KC_NO,KC_NO,k43,KC_NO,KC_NO,KC_NO,k44,  k45,KC_NO}  \
}
