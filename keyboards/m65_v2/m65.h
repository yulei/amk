/**
 * m65.h
 */

#pragma once

#include "amk_keycode.h"

#define LAYOUT_default( \
    k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k0a, k0b, k0c, k0e, \
	k10, k11, k12, k13, k14, k15, k16, k17, k18, k19, k1a, k1b, k1c, k1d, k1e, \
	k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k2a, k2b,      k2d, k2e, \
	k30,      k32, k33, k34, k35, k36, k37, k38, k39, k3a, k3b, k3c, k3d, k3e, \
	k40, k41, k42,                     k46,           k4a, k4b, k4c, k4d, k4e \
) { \
    {k00,  k01, k02,  k03,  k04,  k05,   k06,  k07,  k08,  k09,  k0a, k0b,  k0c, GC_NO, k0e}, \
    {k10,  k11, k12,  k13,  k14,  k15,   k16,  k17,  k18,  k19,  k1a, k1b,  k1c,   k1d, k1e}, \
    {k20,  k21, k22,  k23,  k24,  k25,   k26,  k27,  k28,  k29,  k2a, k2b,GC_NO,   k2d, k2e}, \
    {k30,GC_NO, k32,  k33,  k34,  k35,   k36,  k37,  k38,  k39,  k3a, k3b,  k3c,   k3d, k3e}, \
    {k40,  k41, k42,GC_NO,GC_NO,GC_NO, GC_NO,  k46,GC_NO,GC_NO,  k4a, k4b,  k4c,   k4d, k4e}, \
}
