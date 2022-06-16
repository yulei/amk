/*
 * @file display.c
 */

#include <string.h>
#include <stdbool.h>

#include "timer.h"
#include "led.h"
#include "amk_gpio.h"
#include "usb_descriptors.h"
#include "wait.h"

#ifndef DISP_DEBUG
#define DISP_DEBUG 1
#endif

#if DISP_DEBUG
#define disp_debug  amk_printf
#else
#define disp_debug(...)
#endif

#include "screen.h"
#include "gc9107.h"

static screen_driver_t gc9107_driver;
static screen_driver_param_t gc9107_driver_params = {
    INVALID_PIN,
    SCREEN_0_PWR,
    0,
    SCREEN_0_RESET,
    SCREEN_0_CS,
    SCREEN_0_DC,
    INVALID_PIN,
    INVALID_PIN,
    SPI_LCD_GC9107,
};

static void screen_gc9107_init(void)
{
    gc9107_config(&gc9107_driver, &gc9107_driver_params);
    gc9107_init(&gc9107_driver);
}

static uint32_t last_ticks = 0;

static void set_screen_state(bool enable)
{
    if (enable) {
        gpio_set_output_pushpull(SCREEN_0_PWR);
        gpio_write_pin(SCREEN_0_PWR, SCREEN_0_PWR_EN);
        wait_ms(1);
        //screen_init();
        screen_gc9107_init();
    } else {
        gpio_set_output_pushpull(SCREEN_0_PWR);
        gpio_write_pin(SCREEN_0_PWR, !SCREEN_0_PWR_EN);
        wait_ms(1);
    }
}

//char buffer[80*160*2];

void matrix_init_kb(void)
{
    gpio_set_output_pushpull(SCREEN_0_RESET);
    gpio_write_pin(SCREEN_0_RESET, 0);

    gpio_set_output_pushpull(SCREEN_0_CS);
    gpio_write_pin(SCREEN_0_CS, 0);

    gpio_set_output_pushpull(SCREEN_0_DC);
    gpio_write_pin(SCREEN_0_DC, 0);

    set_screen_state(true);
    last_ticks = timer_read32();
}



void matrix_scan_kb(void)
{
}
