/**
  * @file rabbit60.h
  *
  */
#pragma once

#include "quantum.h"

#define LAYOUT_default( \
  k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0a, k0b, k0c, k0d, k0e, \
	k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1a, k1b, k1c, k1d, \
	k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b,      k2d, \
	k30,      k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d, \
	k40, k41, k42, k43, k44, k45, k46) \
{ \
    {  k09,  k08,k07,  k06,  k05,  k04,  k03,k02,  k01,  k00,KC_NO,KC_NO,k0d,  k0c,k0b,  k0a}, \
    {  k19,  k18,k17,  k16,  k15,  k14,  k13,k12,  k11,  k10,KC_NO,KC_NO,k1d,  k1c,k1b,  k1a}, \
    {  k29,  k28,k27,  k26,  k25,  k24,  k23,k22,  k21,  k20,KC_NO,KC_NO,k2d,KC_NO,k2b,  k2a}, \
    {  k39,  k38,k37,  k36,  k35,  k34,  k33,k32,KC_NO,  k30,KC_NO,KC_NO,k3d,  k3c,k3b,  k3a}, \
    {KC_NO,KC_NO,k43,KC_NO,KC_NO,KC_NO,KC_NO,k42,  k41,  k40,KC_NO,KC_NO,k0e,  k46,k45,  k44}  \
}
