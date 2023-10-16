/**
 * @file rabbit60.c
 */

#include "rabbit60.h"
#include "amk_printf.h"

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "is31fl3236.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, OUT_22, OUT_23, OUT_24},
    {0, OUT_25, OUT_26, OUT_27},
    {0, OUT_28, OUT_29, OUT_30},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 3},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 3},
};
#endif
