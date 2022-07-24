/**
 * pad.h
 */

#pragma once

#include "amk_keycode.h"

#define LAYOUT_default( \
    k00, k01, k02, k03, \
	k10, k11, k12, k13, \
	k20, k21, k22, k23, \
	k30, k31, k32, k33, \
	k40, k41, k42, k43, \
	k50, k51, k52, k53 \
) { \
    {k00, k01, k02, k03}, \
    {k10, k11, k12, k13}, \
    {k20, k21, k22, k23}, \
    {k30, k31, k32, k33}, \
    {k40, k41, k42, k43}, \
    {k50, k51, k52, k53}, \
}
