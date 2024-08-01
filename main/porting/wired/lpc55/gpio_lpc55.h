/**
 * @file gpio_at32.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "amk_hal.h"

#define INVALID_PIN 0xFFFFFFFF

typedef uint32_t pin_t;
typedef uint32_t port_t;

#define MAKE_PIN(port, pin) (((uint32_t)(port)<<16)|((pin)&0xFF))
#define GET_PIN(port_pin) (((port_pin)&0xFF))
#define GET_PORT(port_pin) (((port_pin)&0xFF00)>>16)

#define P0_0    MAKE_PIN(0, 0)
#define P0_1    MAKE_PIN(0, 1)
#define P0_2    MAKE_PIN(0, 2)
#define P0_3    MAKE_PIN(0, 3)
#define P0_4    MAKE_PIN(0, 4)
#define P0_5    MAKE_PIN(0, 5)
#define P0_6    MAKE_PIN(0, 6)
#define P0_7    MAKE_PIN(0, 7)
#define P0_8    MAKE_PIN(0, 8)
#define P0_9    MAKE_PIN(0, 9)
#define P0_10   MAKE_PIN(0, 10)
#define P0_11   MAKE_PIN(0, 11)
#define P0_12   MAKE_PIN(0, 12)
#define P0_13   MAKE_PIN(0, 13)
#define P0_14   MAKE_PIN(0, 14)
#define P0_15   MAKE_PIN(0, 15)
#define P0_16   MAKE_PIN(0, 16)
#define P0_17   MAKE_PIN(0, 17)
#define P0_18   MAKE_PIN(0, 18)
#define P0_19   MAKE_PIN(0, 19)
#define P0_20   MAKE_PIN(0, 20)
#define P0_21   MAKE_PIN(0, 21)
#define P0_22   MAKE_PIN(0, 22)
#define P0_23   MAKE_PIN(0, 23)
#define P0_24   MAKE_PIN(0, 24)
#define P0_25   MAKE_PIN(0, 25)
#define P0_26   MAKE_PIN(0, 26)
#define P0_27   MAKE_PIN(0, 27)
#define P0_28   MAKE_PIN(0, 28)
#define P0_29   MAKE_PIN(0, 29)
#define P0_30   MAKE_PIN(0, 30)
#define P0_31   MAKE_PIN(0, 31)

#define P1_0    MAKE_PIN(1, 0)
#define P1_1    MAKE_PIN(1, 1)
#define P1_2    MAKE_PIN(1, 2)
#define P1_3    MAKE_PIN(1, 3)
#define P1_4    MAKE_PIN(1, 4)
#define P1_5    MAKE_PIN(1, 5)
#define P1_6    MAKE_PIN(1, 6)
#define P1_7    MAKE_PIN(1, 7)
#define P1_8    MAKE_PIN(1, 8)
#define P1_9    MAKE_PIN(1, 9)
#define P1_10   MAKE_PIN(1, 10)
#define P1_11   MAKE_PIN(1, 11)
#define P1_12   MAKE_PIN(1, 12)
#define P1_13   MAKE_PIN(1, 13)
#define P1_14   MAKE_PIN(1, 14)
#define P1_15   MAKE_PIN(1, 15)
#define P1_16   MAKE_PIN(1, 16)
#define P1_17   MAKE_PIN(1, 17)
#define P1_18   MAKE_PIN(1, 18)
#define P1_19   MAKE_PIN(1, 19)
#define P1_20   MAKE_PIN(1, 20)
#define P1_21   MAKE_PIN(1, 21)
#define P1_22   MAKE_PIN(1, 22)
#define P1_23   MAKE_PIN(1, 23)
#define P1_24   MAKE_PIN(1, 24)
#define P1_25   MAKE_PIN(1, 25)
#define P1_26   MAKE_PIN(1, 26)
#define P1_27   MAKE_PIN(1, 27)
#define P1_28   MAKE_PIN(1, 28)
#define P1_29   MAKE_PIN(1, 29)
#define P1_30   MAKE_PIN(1, 30)
#define P1_31   MAKE_PIN(1, 31)