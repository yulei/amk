/**
 * rgb_led.c
 */

#include "rgb_led.h"

void rgb_led_init(void)
{ 
#ifdef RGB_EFFECTS_ENABLE
#ifdef RGB_WITH_WS2812
    rgb_driver_t* driver = rgb_driver_create(RGB_DRIVER_WS2812);
#endif
#ifdef RGB_WITH_AW9523B
    rgb_driver_t* driver = rgb_driver_create(RGB_DRIVER_AW9523B);
#endif
    rgb_effects_init(driver);
#endif
}

void rgb_led_task(void)
{
#ifdef RGB_EFFECTS_ENABLE
    rgb_effects_task();
#endif
}