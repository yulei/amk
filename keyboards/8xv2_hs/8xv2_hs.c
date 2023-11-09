/*
 * @file 8xv2_hs.c
 */

#include "8xv2_hs.h"
#include "rgb_common.h"
#include "rgb_linear.h"
#include "is31fl3731.h"
#include "rgb_driver.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, C1_1,   C3_2,   C4_2},
    {0, C1_2,   C2_2,   C4_3},
    {0, C1_3,   C2_3,   C3_3},
    {0, C1_4,   C2_4,   C3_4},
    {0, C1_5,   C2_5,   C3_5},
    {0, C1_6,   C2_6,   C3_6},
    {0, C1_7,   C2_7,   C3_7},
    {0, C1_8,   C2_8,   C3_8},

    {0, C9_1,   C8_1,   C7_1},
    {0, C9_2,   C8_2,   C7_2},
    {0, C9_3,   C8_3,   C7_3},
    {0, C9_4,   C8_4,   C7_4},
    {0, C9_5,   C8_5,   C7_5},
    {0, C9_6,   C8_6,   C7_6},
    {0, C9_7,   C8_7,   C6_6},
    {0, C9_8,   C7_7,   C6_7},

    {0, C1_9,   C3_10,  C4_10},
    {0, C1_10,  C2_10,  C4_11},
    {0, C1_11,  C2_11,  C3_11},
    {0, C1_12,  C2_12,  C3_12},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3731, 0xE8, 0, 0, 20},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0,  0, 16},
    {1, 16, 4},
};

static bool typing_trigger = false;
static uint32_t typing_ticks;
static amk_hsv_t typing_leds[RGB_LED_NUM];

#define TYPING_UPDATE_INTERVAL  10
#define TYPING_VAL_STEP      8

static void typing_leds_init(void)
{
    for (int i = 0; i < RGB_LED_NUM; i++) {
        typing_leds[i].h = 0;
        typing_leds[i].s = 0;
        typing_leds[i].v = 0;
    }
    typing_ticks = timer_read32();
    srand(typing_ticks);
}

static void typing_leds_pressed(void)
{
    // pick less than 6
    int num = (rand() % 6) + 1;
    for (int i = 0; i < num; i++) {
        int index = rand() % 16;
        if (typing_leds[index].v == 0) {
            typing_leds[index].h = pick_hue();
        }
        typing_leds[index].s = 255;
        typing_leds[index].v = 255;
    }
}

static void typing_leds_update(void)
{
    if (timer_elapsed32(typing_ticks) > TYPING_UPDATE_INTERVAL) {
        for (int i = 0; i < RGB_LED_NUM; i++) {
            if (typing_leds[i].v > TYPING_VAL_STEP) {
                typing_leds[i].v -= TYPING_VAL_STEP;
            } else {
                typing_leds[i].v = 0;
            }
        }
        typing_ticks = timer_read32();
    }
}

static void typing_leds_flush(void)
{
    for (int i = 0; i < 16; i++) {
        amk_rgb_t rgb = hsv_to_rgb_stub(typing_leds[i]);
        rgb_linear_set_rgb(0, i, rgb.r, rgb.g, rgb.b);
    }
    for (int i = 0; i < 4; i++) {
        rgb_linear_set_rgb(1, i, 0, 0, 0);
    }
}

void matrix_init_kb(void)
{
    if (eeconfig_read_kb()) {
        typing_trigger = true;
    } else {
        typing_trigger = false;
    }
    //typing_trigger = true;
    typing_leds_init();
}

void matrix_scan_kb(void)
{
    if( typing_trigger) {
        typing_leds_update();
    }
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record)
{
    if (!record->event.pressed) {
        return true;
    }

    switch(keycode) {
        case KC_F24:
            typing_trigger = !typing_trigger;
            typing_leds_init();
            eeconfig_update_kb((uint32_t)typing_trigger);
            return false;
        default:
            break;
    }

    return true;
}

void rgb_led_pre_flush(void)
{
    if (typing_trigger) {
        typing_leds_flush();
    }
}

void hook_matrix_change_rgb(uint8_t row, uint8_t col, bool pressed)
{
    if (pressed) {
        typing_leds_pressed();
    }
}
