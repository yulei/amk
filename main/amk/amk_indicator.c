/**
 * @file amk_indicator.c
 * @author astro
 * 
 * @copyright Copyright (c) astro 2022
 * 
 */

#include "amk_indicator.h"
#include "amk_gpio.h"

void amk_indicator_init(void)
{
#ifdef CAPS_LED_PIN
    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, !CAPS_LED_ON);
#endif

#ifdef SCROLL_LED_PIN
    gpio_set_output_pushpull(SCROLL_LED_PIN);
    gpio_write_pin(SCROLL_LED_PIN, !SCROLL_LED_ON);
#endif

#ifdef NUM_LED_PIN
    gpio_set_output_pushpull(NUM_LED_PIN);
    gpio_write_pin(NUM_LED_PIN, !NUM_LED_ON);
#endif
}

__attribute__((weak))
void led_set_kb(uint8_t led)
{
}

#define USB_LED_NUM_LOCK                0
#define USB_LED_CAPS_LOCK               1
#define USB_LED_SCROLL_LOCK             2
#define USB_LED_COMPOSE                 3
#define USB_LED_KANA                    4

void led_set(uint8_t led)
{
#ifdef CAPS_LED_PIN
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        gpio_write_pin(CAPS_LED_PIN, CAPS_LED_ON);
    } else {
        gpio_write_pin(CAPS_LED_PIN, !CAPS_LED_ON);
    }
#endif

#ifdef NUM_LED_PIN
    if (led & (1 << USB_LED_NUM_LOCK)) {
        gpio_write_pin(NUM_LED_PIN, NUM_LED_ON);
    } else {
        gpio_write_pin(NUM_LED_PIN, !NUM_LED_ON);
    }
#endif

#ifdef SCROLL_LED_PIN
    if (led & (1 << USB_LED_SCROLL_LOCK)) {
        gpio_write_pin(SCROLL_LED_PIN, SCROLL_LED_ON);
    } else {
        gpio_write_pin(SCROLL_LED_PIN, !SCROLL_LED_ON);
    }
#endif
    led_set_kb(led);
}