/**
 * rgb_led.c
 */

#include "rgb_led.h"

void rgb_led_init(void)
{ 
#if defined(RGB_EFFECTS_ENABLE) || defined(RGB_MATRIX_ENABLE)
    rgb_driver_t *driver = 0;
#endif

#ifdef RGB_EFFECTS_ENABLE
#ifdef RGB_WITH_ALL

    if (rgb_driver_available(RGB_DRIVER_AW9523B)) {
        driver = rgb_driver_create(RGB_DRIVER_AW9523B);
    } else {
        driver = rgb_driver_create(RGB_DRIVER_WS2812);
    }

#else
    #ifdef RGB_WITH_WS2812
        driver = rgb_driver_create(RGB_DRIVER_WS2812);
    #endif
    #ifdef RGB_WITH_AW9523B
        driver = rgb_driver_create(RGB_DRIVER_AW9523B);
    #endif
#endif
    rgb_effects_init(driver);
#endif

#ifdef RGB_MATRIX_ENABLE
    driver = rgb_driver_create(RGB_DRIVER_IS31FL3731);
    rgb_matrix_init(driver);
#endif
}

void rgb_led_task(void)
{
#ifdef RGB_EFFECTS_ENABLE
    rgb_effects_task();
#endif
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_task();
#endif
}