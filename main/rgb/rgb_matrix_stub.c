/**
 * @file rgb_matrix_stub.c
 */

#include "rgb_matrix_stub.h"
#include "rgb_driver.h"
#include "rgb_led.h"

#ifdef CUSTOMRGB_ENABLE
#include "customrgb.h"
#endif

//rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM];

void rgb_matrix_init_stub(void)
{
#ifdef CUSTOMRGB_ENABLE
    customrgb_init();
#endif
    // initialize rgb matrix 
    for (int i = 0; i < RGB_MATRIX_NUM; i++) {
        uint8_t config = g_rgb_matrix_params[i].config;
        g_rgb_configs[config].index = config;
        g_rgb_configs[config].type = RGB_EFFECT_MATRIX;
        g_rgb_matrix_params[i].config = config;
        g_rgb_matrix_params[i].led_start = g_rgb_matrix_params[i].led_start;
        g_rgb_matrix_params[i].led_num = g_rgb_matrix_params[i].led_num;
        rgb_matrix_init();
    }

    //rgb_matrix_mode_noeeprom(RGB_MATRIX_ALPHAS_MODS);
}

void rgb_matrix_task_stub(void)
{
    for (int i = 0; i < RGB_MATRIX_NUM; i++) {
        rgb_matrix_task();
    }
}

void rgb_matrix_uninit_stub(void)
{
}

void rgb_matrix_prepare_sleep_stub(void)
{}

bool rgb_matrix_enabled(uint8_t index)
{
    return rgb_matrix_is_enabled() != 0;
}

void rgb_matrix_set_rgb_stub(uint8_t index, uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
    rgb_matrix_set_color(led, r, g, b);
}

static void custom_driver_init(void)
{}

static void custom_driver_set_color(int index, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t g_index = index + g_rgb_matrix_params[0].led_start;
    rgb_driver_t *driver = rgb_led_map(g_index);
    if (driver) {
        driver->set_color_rgb(driver, g_index, r, g, b);
        //driver->set_color(driver, g_index, 255, 255, 255);
    }
}

static void custom_driver_set_color_all(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint8_t i = 0; i < g_rgb_matrix_params[0].led_num; i++) {
        custom_driver_set_color(i, r, g, b);
    }
}

static void custom_driver_flush(void)
{
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init = custom_driver_init,
    .flush = custom_driver_flush,
    .set_color = custom_driver_set_color,
    .set_color_all = custom_driver_set_color_all,
};

#include "quantum.h"
bool process_rgb(const uint16_t keycode, const keyrecord_t *record)
{
    return true;
}

void rgb_matrix_step_stub(void)
{
    rgb_matrix_step();
#ifdef SIGNALRGB_ENABLE
    if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_SIGNALRGB) {
        rgb_matrix_step();
    }
#endif

#ifdef OPENRGB_ENABLE
    if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_OPENRGB) {
        rgb_matrix_step();
    }
#endif
}

void rgb_matrix_step_reverse_stub(void)
{
    rgb_matrix_step_reverse();
#ifdef SIGNALRGB_ENABLE
    if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_SIGNALRGB) {
        rgb_matrix_step_reverse();
    }
#endif

#ifdef OPENRGB_ENABLE
    if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_OPENRGB) {
        rgb_matrix_step_reverse();
    }
#endif
}