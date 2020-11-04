/**
 * amk_printf.h
 */
#pragma once

#include "printf.h"

#define amk_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
