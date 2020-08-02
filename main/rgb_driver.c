/**
 * rgb_driver.c
 *  rgb led driver implementation
 */

#include <stddef.h>
#include "rgb_driver.h"
#include "rgb_color.h"

#include "drivers/ws2812.h"
#include "drivers/aw9523b.h"

#ifndef RGB_LED_NUM
#error "RGB_LED_NUM must be defined"
#endif

static rgb_led_t rgb_leds[RGB_LED_NUM];

static void rd_ws2812_init(void);
static void rd_ws2812_uninit(void);
static void rd_ws2812_set_color(uint32_t index, uint8_t red, uint8_t blue, uint8_t green);
static void rd_ws2812_set_color_all(uint8_t red, uint8_t blue, uint8_t green);
static void rd_ws2812_flush(void);

static rgb_driver_t ws2812_driver = {
    .init = rd_ws2812_init,
    .uninit = rd_ws2812_uninit,
    .set_color = rd_ws2812_set_color,
    .set_color_all = rd_ws2812_set_color_all,
    .flush = rd_ws2812_flush,
};

static void rd_aw9523b_init(void);
static void rd_aw9523b_uninit(void);
static void rd_aw9523b_set_color(uint32_t index, uint8_t red, uint8_t blue, uint8_t green);
static void rd_aw9523b_set_color_all(uint8_t red, uint8_t blue, uint8_t green);
static void rd_aw9523b_flush(void);

static rgb_driver_t aw9523b_driver = {
    .init = rd_aw9523b_init,
    .uninit = rd_aw9523b_uninit,
    .set_color = rd_aw9523b_set_color,
    .set_color_all = rd_aw9523b_set_color_all,
    .flush = rd_aw9523b_flush,
};

rgb_driver_t* rgb_driver_create(RGB_DRIVER_TYPE type)
{
    switch(type) {
        case RGB_DRIVER_WS2812:
            return &ws2812_driver;
        case RGB_DRIVER_AW9523B:
            return &aw9523b_driver;
        default:
            break;
    }
    return NULL;
}

void rgb_driver_destroy(rgb_driver_t* driver)
{
    (void)driver;
}

void rd_ws2812_init(void)
{
    ws2812_init();
}

void rd_ws2812_uninit(void)
{
    ws2812_uninit();
}

void rd_ws2812_set_color(uint32_t index, uint8_t red, uint8_t blue, uint8_t green)
{
    if( index < RGB_LED_NUM) {
        rgb_leds[index].r = red;
        rgb_leds[index].g = green;
        rgb_leds[index].b = blue;
    }
}

void rd_ws2812_set_color_all(uint8_t red, uint8_t blue, uint8_t green)
{
    for (int i = 0; i < RGB_LED_NUM; i++) {
        rgb_leds[i].r = red;
        rgb_leds[i].g = green;
        rgb_leds[i].b = blue;
    }
}

void rd_ws2812_flush(void)
{
    ws2812_setleds(rgb_leds, RGB_LED_NUM);
}

void rd_aw9523b_init(void)
{
    aw9523b_init(AW9523B_ADDR);
}

void rd_aw9523b_uninit(void)
{
    aw9523b_uninit(AW9523B_ADDR);
}

void rd_aw9523b_set_color(uint32_t index, uint8_t red, uint8_t blue, uint8_t green)
{
    aw9523b_set_color(index, red, green, blue);
}

void rd_aw9523b_set_color_all(uint8_t red, uint8_t blue, uint8_t green)
{
    aw9523b_set_color_all(red, blue, green);
}

void rd_aw9523b_flush(void)
{
    aw9523b_update_pwm_buffers(AW9523B_ADDR);
}