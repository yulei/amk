/**
 * @file ms_navi.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "ms_navi.h"

#include "is31fl3236.h"
#include "rgb_led.h"

#ifdef RGB_ENABLE

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, OUT_36, OUT_35, OUT_34},
    {0, OUT_33, OUT_32, OUT_31},
    {0, OUT_30, OUT_29, OUT_28},
//    {0, OUT_27, OUT_26, OUT_25},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 3},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 3},
//    {1, 3, 1},
};

#endif

#if APC_SPECIAL_KEY_NUM
#include "amk_apc.h"
struct apc_special_key apc_special_keys[APC_SPECIAL_KEY_NUM] = {
    {3, 12, 2150, 2550, 600, 100}, // caps
};
#endif
