/**
 * @file rgb_led.c
 */

#include "rgb_led.h"
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "rgb_indicator.h"
#include "rgb_matrix.h"

#ifdef RGB_LINEAR_ENABLE
    #define RGB_LINEAR_CONFIG_NUM          RGB_SEGMENT_NUM 
#else
    #define RGB_LINEAR_CONFIG_NUM          0
#endif

#ifdef RGB_INDICATOR_ENABLE
    #define RGB_INDICATOR_CONFIG_NUM    RGB_INDICATOR_NUM
#else
    #define RGB_INDICATOR_CONFIG_NUM    0
#endif

#ifdef RGB_MATRIX_ENABLE
    #define RGB_MATRIX_CONFIG_NUM       RGB_MATRIX_NUM
#else
    #define RGB_MATRIX_CONFIG_NUM       0
#endif

#define RGB_LED_CONFIG_NUM              (RGB_LINEAR_CONFIG_NUM+RGB_INDICATOR_CONFIG_NUM+RGB_MATRIX_CONFIG_NUM)

rgb_config_t g_rgb_configs[RGB_LED_CONFIG_NUM];

void rgb_led_init(void)
{ 
    rgb_driver_init();

#ifdef RGB_LINEAR_ENABLE
    rgb_linear_init();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_init();
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_init();
#endif
}

void rgb_led_task(void)
{
#ifdef RGB_LINEAR_ENABLE
    rgb_linear_task();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_task();
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