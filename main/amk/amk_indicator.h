/**
 * @file amk_indicator.h
 * @author astro
 * 
 * @copyright Copyright (c) astro 2022
 * 
 */

#pragma once

#include <stdint.h>
#include "led.h"

#ifndef CAPS_LED_ON
#define CAPS_LED_ON     1
#endif

#ifndef NUM_LED_ON
#define NUM_LED_ON      1
#endif

#ifndef SCROLL_LED_ON
#define SCROLL_LED_ON   1
#endif

void amk_indicator_init(void);