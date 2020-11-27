/**
 * rgb_driver.c
 *  rgb led driver implementation
 */

#include <stddef.h>
#include "rgb_driver.h"
#include "rgb_color.h"
#include "gpio_pin.h"
#include "wait.h"

#include "drivers/ws2812.h"
#include "drivers/aw9523b.h"
#include "drivers/is31fl3731.h"

#ifndef RGB_LED_NUM
#error "RGB_LED_NUM must be defined"
#endif

#ifdef RGB_WITH_WS2812
static void rd_ws2812_init(void);
static void rd_ws2812_uninit(void);
static void rd_ws2812_set_color(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);
static void rd_ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
static void rd_ws2812_flush(void);

static rgb_driver_t ws2812_driver = {
    .init = rd_ws2812_init,
    .uninit = rd_ws2812_uninit,
    .set_color = rd_ws2812_set_color,
    .set_color_all = rd_ws2812_set_color_all,
    .flush = rd_ws2812_flush,
};

void rd_ws2812_init(void)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    pin_t p = WS2812_LED_PIN;
    ws2812_init(p);
}

void rd_ws2812_uninit(void)
{
    pin_t p = WS2812_LED_PIN;
    ws2812_uninit(p);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
}

void rd_ws2812_set_color(uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    ws2812_set_color(index, rgb.r, rgb.g, rgb.b);
}

void rd_ws2812_set_color_all(uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    ws2812_set_color_all(rgb.r, rgb.g, rgb.b);
}

void rd_ws2812_flush(void)
{
    pin_t p = WS2812_LED_PIN;
    ws2812_update_buffers(p);
}

#endif
#ifdef RGB_WITH_AW9523B
static void rd_aw9523b_init(void);
static void rd_aw9523b_uninit(void);
static void rd_aw9523b_set_color(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);
static void rd_aw9523b_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
static void rd_aw9523b_flush(void);

static rgb_driver_t aw9523b_driver = {
    .init = rd_aw9523b_init,
    .uninit = rd_aw9523b_uninit,
    .set_color = rd_aw9523b_set_color,
    .set_color_all = rd_aw9523b_set_color_all,
    .flush = rd_aw9523b_flush,
};

void rd_aw9523b_init(void)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    aw9523b_init(AW9523B_ADDR);
}

void rd_aw9523b_uninit(void)
{
    aw9523b_uninit(AW9523B_ADDR);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
}

void rd_aw9523b_set_color(uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    aw9523b_set_color(index, rgb.r, rgb.g, rgb.b);
}

void rd_aw9523b_set_color_all(uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    aw9523b_set_color_all(rgb.r, rgb.g, rgb.b);
}

void rd_aw9523b_flush(void)
{
    aw9523b_update_buffers(AW9523B_ADDR);
}
#endif

#ifdef RGB_WITH_IS31FL3731

static void rd_3731_init(void);
static void rd_3731_uninit(void);
static void rd_3731_set_color(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);
static void rd_3731_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
static void rd_3731_flush(void);

static void map_is31_led(uint8_t index, uint8_t *red_reg, uint8_t* green_reg, uint8_t *blue_reg);

static rgb_driver_t is31fl3731_driver = {
    .init           = rd_3731_init;
    .uninit         = rd_3731_uninit;
    .set_color      = rd_3731_set_color;
    .set_color_all  = rd_3731_set_color_all;
    .flush          = rd_3731_flush;
};

static is31fl3731_t is31_drivers[IS31_DRIVER_NUM];


static void rd_3731_init(void)
{
#ifdef IS31_ADDR1
    is31_drivers[0] = is31fl3731_init(IS31_ADDR1, IS31_LED_NUM1, map_is31_led);
#endif
#ifdef IS31_ADDR2
    is31_drivers[1] = is31fl3731_init(IS31_ADDR2, IS31_LED_NUM2, map_is31_led);
#endif
}

static void rd_3731_uninit(void)
{
#ifdef IS31_ADDR1
    is31fl3731_uninit(is31_drivers[0]);
#endif
#ifdef IS31_ADDR2
    is31fl3731_uninit(is31_drivers[1]);
#endif
}

static void rd_3731_set_color(uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    rgb_matrix_led_t *led = &g_rgb_matrix_leds.leds[index];
    is31_drivers[led->driver]->set_color(index, rgb.r, rgb.g, rgb.b);
}

static void rd_3731_set_color_all(uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
#ifdef IS31_ADDR1
    is31fl3731_set_color_all(is31_drivers[0], rgb.r, rgb.g, rgb.b);
#endif
#ifdef IS31_ADDR2
    is31fl3731_set_color_all(is31_drivers[1], rgb.r, rgb.g, rgb.b);
#endif
}

static void rd_3731_flush(void)
{
#ifdef IS31_ADDR1
    is31fl3731_update_buffers(is31_drivers[0]);
#endif
#ifdef IS31_ADDR2
    is31fl3731_update_buffers(is31_drivers[1]);
#endif
}

static void map_is31_led(uint8_t index, uint8_t *red_reg, uint8_t* green_reg, uint8_t *blue_reg)
{
    rgb_matrix_led_t *led = &g_rgb_matrix_leds.leds[index];
    *red_reg    = led->red;
    *green_reg  = led->green;
    *blue_reg   = led->blue;
}

#endif

rgb_driver_t* rgb_driver_create(RGB_DRIVER_TYPE type)
{
    switch(type) {
#ifdef RGB_WITH_WS2812
        case RGB_DRIVER_WS2812:
            return &ws2812_driver;
#endif
#ifdef RGB_WITH_AW9523B
        case RGB_DRIVER_AW9523B:
            return &aw9523b_driver;
#endif
#ifdef RGB_WITH_IS31FL3731
        case RGB_DRIVER_IS31FL3731:
            
#endif
        default:
            break;
    }
    return NULL;
}

void rgb_driver_destroy(rgb_driver_t* driver)
{
    //driver->uninit();
}
