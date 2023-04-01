/**
 * onekey.h
 */

#pragma once

#include "quantum.h"

#define LAYOUT_default(k00) { {k00} }

#define LAYOUT_ext(k00, k01, k02, k10, k11, k12) { {k00, k01, k02}, {k10, k11, k12} }