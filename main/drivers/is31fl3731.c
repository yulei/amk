/**
 * is31fl3731.c
 *  driver implementation for is31fl3731
 */

#include "is31fl3731.h"

#ifndef IS31_DRIVER_NUM
#error "IS31_DRIVER_NUM must defined"
#endif

static is31fl3731_t is32fl3731_drivers[IS31_DRIVER_NUM];

is31fl3731_t* is31fl3731_init(uint8_t addr, uint8_t led_num, map_led_t map_led)
{}

void is31fl3731_set_color(is31fl3731_t* driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{}

void is31fl3731_set_color_all(is31fl3731_t* driver, uint8_t red, uint8_t green, uint8_t blue)
{}

void is31fl3731_update_buffers(is31fl3731_t* driver)
{}

void is31fl3731_uninit(is31fl3731_t *led_driver)
{}