/**
 * @file rgb_driver.c
 * @breif rgb led driver implementation
 */

#include <stddef.h>
#include "rgb_driver.h"
#include "gpio_pin.h"
#include "wait.h"
#include "amk_printf.h"

#include "drivers/ws2812.h"
#include "drivers/aw9523b.h"
#include "drivers/aw9106b.h"
#include "drivers/is31fl3731.h"
#include "drivers/is31fl3236.h"
#include "drivers/is31fl3733.h"

#ifndef RGB_DRIVER_DEBUG
#define RGB_DRIVER_DEBUG 1
#endif

#if RGB_DRIVER_DEBUG
#define rgb_driver_debug  amk_printf
#else
#define rgb_driver_debug(...)
#endif

rgb_driver_t rgb_drivers[RGB_DEVICE_NUM];

static void rd_ws2812_init(rgb_driver_t *driver)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    pin_t p = WS2812_LED_PIN;
    ws2812_init(p);
    driver->data = NULL;
}

static void rd_ws2812_uninit(rgb_driver_t *driver)
{
    pin_t p = WS2812_LED_PIN;
    ws2812_uninit(p);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
}

static void rd_ws2812_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    ws2812_set_color(index, rgb.r, rgb.g, rgb.b);
}

static void rd_ws2812_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    ws2812_set_color_all(rgb.r, rgb.g, rgb.b);
}

static void rd_ws2812_flush(rgb_driver_t *driver)
{
    pin_t p = WS2812_LED_PIN;
    ws2812_update_buffers(p);
}

static void rd_aw9523b_init(rgb_driver_t *driver)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    driver->data = aw9523b_init(driver->device->addr, driver->device->index, driver->device->led_count);
    rgb_driver_debug("AW9523B init\n");
}

static void rd_aw9523b_uninit(rgb_driver_t *driver)
{
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_uninit(awinic);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
    rgb_driver_debug("AW9523B uninit\n");
}

static void rd_aw9523b_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_set_color(awinic, index, rgb.r, rgb.g, rgb.b);
}

static void rd_aw9523b_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_set_color_all(awinic, rgb.r, rgb.g, rgb.b);
}

static void rd_aw9523b_flush(rgb_driver_t *driver)
{
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_update_buffers(awinic);
}

static void rd_aw9106b_init(rgb_driver_t *driver)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    driver->data = aw9106b_init(driver->device->addr, driver->device->index, driver->device->led_count);
    rgb_driver_debug("AW9106B init\n");
}

static void rd_aw9106b_uninit(rgb_driver_t *driver)
{
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9106b_uninit(awinic);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
    rgb_driver_debug("AW106B uninit\n");
}

static void rd_aw9106b_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_set_color(awinic, index, rgb.r, rgb.g, rgb.b);
}

static void rd_aw9106b_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_set_color_all(awinic, rgb.r, rgb.g, rgb.b);
}

static void rd_aw9106b_flush(rgb_driver_t *driver)
{
    awinic_t *awinic = (awinic_t*)driver->data;
    aw9523b_update_buffers(awinic);
}

static void rd_3731_init(rgb_driver_t *driver)
{
    driver->data = is31fl3731_init(driver->device->addr, driver->device->index, driver->device->led_count);
}

static void rd_3731_uninit(rgb_driver_t *driver)
{
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3731_uninit(is31);
}

static void rd_3731_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3731_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3731_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3731_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static void rd_3731_flush(rgb_driver_t *driver)
{
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3731_update_buffers(is31);
}

static void rd_3733_init(rgb_driver_t *driver)
{
    driver->data = is31fl3733_init(driver->device->addr, driver->device->index, driver->device->led_count);
}

static void rd_3733_uninit(rgb_driver_t *driver)
{
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3733_uninit(is31);
}

static void rd_3733_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3733_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3733_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3733_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static void rd_3733_flush(rgb_driver_t *driver)
{
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3733_update_buffers(is31);
}

static void rd_3236_init(rgb_driver_t *driver)
{
    driver->data = is31fl3236_init(driver->device->addr, driver->device->index, driver->device->led_count);
}

static void rd_3236_uninit(rgb_driver_t *driver)
{
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3236_uninit(is31);
}

static void rd_3236_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3236_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3236_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    hsv_t hsv = {hue, sat, val};
    rgb_t rgb = hsv_to_rgb(hsv);
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3236_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static void rd_3236_flush(rgb_driver_t *driver)
{
    is31_t *is31 = (is31_t*)driver->data;
    is31fl3236_update_buffers(is31);
}

bool rgb_driver_available(rgb_driver_type_t type)
{
    switch(type) {
        case RGB_DRIVER_WS2812:
            return true;    // always available
        case RGB_DRIVER_AW9523B:
            return aw9523b_available(AW9523B_ADDR);
        case RGB_DRIVER_AW9106B:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3731:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3733:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3236:
            return true;    // TODO
        default:
            break;
    }
    return true;
}

rgb_driver_t* rgb_driver_create(rgb_device_t *device)
{
    rgb_driver_t *driver = &rgb_drivers[device->index];
    driver->device = device;
    driver->data = NULL;
    switch(device->type) {
        case RGB_DRIVER_WS2812:
            driver->init = rd_ws2812_init;
            driver->set_color = rd_ws2812_set_color;
            driver->set_color_all = rd_ws2812_set_color_all;
            driver->flush = rd_ws2812_flush;
            driver->uninit = rd_ws2812_uninit;
            break;
        case RGB_DRIVER_AW9523B:
            driver->init = rd_aw9523b_init;
            driver->set_color = rd_aw9523b_set_color;
            driver->set_color_all = rd_aw9523b_set_color_all;
            driver->flush = rd_aw9523b_flush;
            driver->uninit = rd_aw9523b_uninit;
            break;
        case RGB_DRIVER_AW9106B:
            driver->init = rd_aw9106b_init;
            driver->set_color = rd_aw9106b_set_color;
            driver->set_color_all = rd_aw9106b_set_color_all;
            driver->flush = rd_aw9106b_flush;
            driver->uninit = rd_aw9106b_uninit;
            break;
        case RGB_DRIVER_IS31FL3731:
            driver->init = rd_3731_init;
            driver->set_color = rd_3731_set_color;
            driver->set_color_all = rd_3731_set_color_all;
            driver->flush = rd_3731_flush;
            driver->uninit = rd_3731_uninit;
            break;
        case RGB_DRIVER_IS31FL3733:
            driver->init = rd_3733_init;
            driver->set_color = rd_3733_set_color;
            driver->set_color_all = rd_3733_set_color_all;
            driver->flush = rd_3733_flush;
            driver->uninit = rd_3733_uninit;
            break;
        case RGB_DRIVER_IS31FL3236:
            driver->init = rd_3236_init;
            driver->set_color = rd_3236_set_color;
            driver->set_color_all = rd_3236_set_color_all;
            driver->flush = rd_3236_flush;
            driver->uninit = rd_3236_uninit;
            break;
        default:
            return NULL;
    }

    return driver;
}

void rgb_driver_destroy(rgb_driver_t* driver)
{
    //driver->uninit();
}

void rgb_driver_init(void)
{
	for (int i = 0; i < RGB_DEVICE_NUM; i++) {
        rgb_driver_t *driver = rgb_driver_create(&g_rgb_devices[i]);
        driver->init(driver);
        rgb_drivers[i].data = driver;
	}
} 

rgb_driver_t *rgb_driver_get(uint8_t index)
{
    rgb_driver_t *driver = NULL;
    if (index < RGB_DEVICE_NUM) {
        driver = &rgb_drivers[index];
    }
    return driver;
}