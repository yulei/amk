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

#ifndef RGB_LED_NUM
#error "RGB_LED_NUM must be defined"
#endif

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
    ws2812_init(WS2812_LED_PIN);
}

void rd_ws2812_uninit(void)
{
    ws2812_uninit(WS2812_LED_PIN);
}

void rd_ws2812_set_color(uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    ws2812_set_color(index, red, green, blue);
}

void rd_ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    ws2812_set_color_all(red, green, blue);
}

void rd_ws2812_flush(void)
{
    ws2812_update_buffers(WS2812_LED_PIN);
}

void rd_aw9523b_init(void)
{
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
    aw9523b_init(AW9523B_ADDR);
}

void rd_aw9523b_uninit(void)
{
    aw9523b_uninit(AW9523B_ADDR);
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
}

void rd_aw9523b_set_color(uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    aw9523b_set_color(index, red, green, blue);
}

void rd_aw9523b_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    aw9523b_set_color_all(red, green, blue);
}

void rd_aw9523b_flush(void)
{
    aw9523b_update_buffers(AW9523B_ADDR);
}