/**
 * rgb_led.c
 */

#include "rgb_led.h"
#include "rgb_driver.h"
#include "rgb_bar.h"
#include "rgb_indicator.h"
#include "rgb_ring.h"
#include "rgb_matrix.h"

void rgb_led_init(void)
{ 
    rgb_driver_init();

#ifdef RGB_BAR_ENABLE
    rgb_bar_init();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_init();
#endif

#ifdef RGB_RING_ENABLE
    rgb_ring_init();
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_init();
#endif
}

void rgb_led_task(void)
{
#ifdef RGB_BAR_ENABLE
    rgb_bar_task();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_task();
#endif

#ifdef RGB_RING_ENABLE
    rgb_ring_task();
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_task();
#endif
    for (int i = 0; i < RGB_DEVICE_NUM; i++) {
        rgb_driver_t *driver = rgb_driver_get(i);
        driver->flush(driver);
    }
}

rgb_driver_t *rgb_led_map(uint8_t led_index)
{
    uint8_t driver = g_rgb_leds[led_index].driver;

    return rgb_driver_get(driver);
}