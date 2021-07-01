/**
 * @file rgb_indicator.h
 */

#include "rgb_indicator.h"
#include "rgb_driver.h"
#include "amk_gpio.h"
#include "wait.h"

void rgb_indicator_init(void)
{
    for (int i = 0; i < RGB_INDICATOR_LED_NUM; i++) {
        rgb_indicator_set(i, 0, 0, 0);
    }
}

void rgb_indicator_set(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t g_index = g_rgb_indicator_index[index];
    rgb_driver_t *driver = rgb_driver_get(g_rgb_leds[g_index].driver);

    if (driver) {
        driver->set_color_rgb(driver, g_index, red, green, blue);
        driver->flush(driver);
    }
}

void rgb_indicator_task(void)
{
}

void rgb_indicator_uninit(void)
{
}

void rgb_indicator_prepare_sleep(void)
{
    #ifdef INDICATOR_SHDN_PIN
        gpio_set_output_pushpull(INDICATOR_SHDN_PIN);
        gpio_write_pin(INDICATOR_SHDN_PIN, 0);
        wait_ms(1);
    #endif
}