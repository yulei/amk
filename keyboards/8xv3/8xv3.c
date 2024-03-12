/*
 * @file 8xv3.c
 */

#include "8xv3.h"
#include "rgb_common.h"
#include "is31fl3741.h"
#include "is31fl3731.h"
#include "is31fl3236.h"
#include "rgb_driver.h"

#ifdef RGB_ENABLE
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, CS23_SW9, CS22_SW9, CS24_SW9},
    {0, CS23_SW8, CS22_SW8, CS24_SW8},
    {0, CS23_SW7, CS22_SW7, CS24_SW7},
    {0, CS23_SW6, CS22_SW6, CS24_SW6},
    {0, CS23_SW5, CS22_SW5, CS24_SW5},
    {0, CS23_SW4, CS22_SW4, CS24_SW4},
    {0, CS23_SW3, CS22_SW3, CS24_SW3},
    {0, CS23_SW2, CS22_SW2, CS24_SW2},
    {0, CS23_SW1, CS22_SW1, CS24_SW1},

    {0, CS20_SW9, CS19_SW9, CS21_SW9},
    {0, CS20_SW8, CS19_SW8, CS21_SW8},
    {0, CS20_SW7, CS19_SW7, CS21_SW7},
    {0, CS20_SW6, CS19_SW6, CS21_SW6},
    {0, CS20_SW5, CS19_SW5, CS21_SW5},
    {0, CS20_SW4, CS19_SW4, CS21_SW4},
    {0, CS20_SW3, CS19_SW3, CS21_SW3},
    {0, CS20_SW2, CS19_SW2, CS21_SW2},
    {0, CS20_SW1, CS19_SW1, CS21_SW1},

    {0, CS17_SW9, CS16_SW9, CS18_SW9},
    {0, CS17_SW8, CS16_SW8, CS18_SW8},
    {0, CS17_SW7, CS16_SW7, CS18_SW7},
    {0, CS17_SW6, CS16_SW6, CS18_SW6},
    {0, CS17_SW5, CS16_SW5, CS18_SW5},
    {0, CS17_SW4, CS16_SW4, CS18_SW4},
    {0, CS17_SW3, CS16_SW3, CS18_SW3},
    {0, CS17_SW2, CS16_SW2, CS18_SW2},
    {0, CS17_SW1, CS16_SW1, CS18_SW1},

    {0, CS11_SW9, CS10_SW9, CS12_SW9},
    {0, CS11_SW8, CS10_SW8, CS12_SW8},
    {0, CS11_SW7, CS10_SW7, CS12_SW7},
    {0, CS11_SW6, CS10_SW6, CS12_SW6},
    {0, CS11_SW5, CS10_SW5, CS12_SW5},
    {0, CS11_SW4, CS10_SW4, CS12_SW4},
    {0, CS11_SW3, CS10_SW3, CS12_SW3},
    {0, CS11_SW2, CS10_SW2, CS12_SW2},
    {0, CS11_SW1, CS10_SW1, CS12_SW1},

    {0,  CS8_SW9,  CS7_SW9,  CS9_SW9},
    {0,  CS8_SW8,  CS7_SW8,  CS9_SW8},
    {0,  CS8_SW7,  CS7_SW7,  CS9_SW7},
    {0,  CS8_SW6,  CS7_SW6,  CS9_SW6},
    {0,  CS8_SW5,  CS7_SW5,  CS9_SW5},
    {0,  CS8_SW4,  CS7_SW4,  CS9_SW4},
    {0,  CS8_SW3,  CS7_SW3,  CS9_SW3},
    {0,  CS8_SW2,  CS7_SW2,  CS9_SW2},
    {0,  CS8_SW1,  CS7_SW1,  CS9_SW1},

    {0,  CS2_SW9,  CS1_SW9,  CS3_SW9},
    {0,  CS2_SW8,  CS1_SW8,  CS3_SW8},
    {0,  CS2_SW7,  CS1_SW7,  CS3_SW7},
    {0,  CS2_SW6,  CS1_SW6,  CS3_SW6},
    {0,  CS2_SW5,  CS1_SW5,  CS3_SW5},
    {0,  CS2_SW4,  CS1_SW4,  CS3_SW4},
    {0,  CS2_SW3,  CS1_SW3,  CS3_SW3},
    {0,  CS2_SW2,  CS1_SW2,  CS3_SW2},
    {0,  CS2_SW1,  CS1_SW1,  CS3_SW1},

    {0, CS29_SW9, CS28_SW9, CS30_SW9},
    {0, CS29_SW8, CS28_SW8, CS30_SW8},
    {0, CS29_SW7, CS28_SW7, CS30_SW7},
    {0, CS29_SW6, CS28_SW6, CS30_SW6},
    {0, CS29_SW5, CS28_SW5, CS30_SW5},
    {0, CS29_SW4, CS28_SW4, CS30_SW4},
    {0, CS29_SW3, CS28_SW3, CS30_SW3},
    //{0, CS29_SW2, CS28_SW2, CS30_SW2},
    {0, CS29_SW1, CS28_SW1, CS30_SW1},

    {0, CS26_SW9, CS25_SW9, CS27_SW9},
    {0, CS26_SW8, CS25_SW8, CS27_SW8},
    {0, CS26_SW7, CS25_SW7, CS27_SW7},
    {0, CS26_SW6, CS25_SW6, CS27_SW6},
    {0, CS26_SW5, CS25_SW5, CS27_SW5},
    {0, CS26_SW4, CS25_SW4, CS27_SW4},
    {0, CS26_SW3, CS25_SW3, CS27_SW3},
    {0, CS26_SW2, CS25_SW2, CS27_SW2},
    {0, CS26_SW1, CS25_SW1, CS27_SW1},

    {0, CS14_SW9, CS13_SW9, CS15_SW9},
    {0, CS14_SW8, CS13_SW8, CS15_SW8},
    {0, CS14_SW7, CS13_SW7, CS15_SW7},
    {0, CS14_SW6, CS13_SW6, CS15_SW6},
    {0, CS14_SW5, CS13_SW5, CS15_SW5},
    {0, CS14_SW4, CS13_SW4, CS15_SW4},
    {0, CS14_SW3, CS13_SW3, CS15_SW3},
    {0, CS14_SW2, CS13_SW2, CS15_SW2},
    {0, CS14_SW1, CS13_SW1, CS15_SW1},

    {0,  CS5_SW9,  CS4_SW9,  CS6_SW9},
    {0,  CS5_SW8,  CS4_SW8,  CS6_SW8},
    {0,  CS5_SW7,  CS4_SW7,  CS6_SW7},
    {0,  CS5_SW6,  CS4_SW6,  CS6_SW6},
    {0,  CS5_SW5,  CS4_SW5,  CS6_SW5},
    {0,  CS5_SW4,  CS4_SW4,  CS6_SW4},
    {0,  CS5_SW3,  CS4_SW3,  CS6_SW3},
    {0,  CS5_SW2,  CS4_SW2,  CS6_SW2},
    //{0,  CS5_SW1,  CS4_SW1,  CS6_SW1},

    {0, CS32_SW9, CS31_SW9, CS33_SW9},
    {0, CS32_SW8, CS31_SW8, CS33_SW8},
    {0, CS32_SW7, CS31_SW7, CS33_SW7},
    {0, CS32_SW6, CS31_SW6, CS33_SW6},
    {0, CS32_SW5, CS31_SW5, CS33_SW5},
    {0, CS32_SW4, CS31_SW4, CS33_SW4},
    {0, CS32_SW3, CS31_SW3, CS33_SW3},
    {0, CS32_SW2, CS31_SW2, CS33_SW2},
    {0, CS32_SW1, CS31_SW1, CS33_SW1},

#ifdef RABBIT_VER
    // 6 leds
    {1, OUT_24, OUT_23, OUT_22},
    {1, OUT_21, OUT_20, OUT_19},
    {1, OUT_18, OUT_17, OUT_16},
    {1, OUT_15, OUT_14, OUT_13},
    {1, OUT_12, OUT_11, OUT_10},
    {1,  OUT_9,  OUT_8,  OUT_7},
#elif defined(LED_SCREEN_VER)
    {1, C1_1,   C3_2,   C4_2},
    {1, C1_2,   C2_2,   C4_3},
    {1, C1_3,   C2_3,   C3_3},
    {1, C1_4,   C2_4,   C3_4},
    {1, C1_8,   C2_8,   C3_8},
    {1, C9_4,   C8_4,   C7_4},
    {1, C9_8,   C7_7,   C6_7},
    {1, C9_7,   C8_7,   C6_6},
    {1, C9_6,   C8_6,   C7_6},
    {1, C9_5,   C8_5,   C7_5},
    {1, C9_1,   C8_1,   C7_1},
    {1, C1_5,   C2_5,   C3_5},

    {1, C1_6,   C2_6,   C3_6},
    {1, C1_7,   C2_7,   C3_7},
    {1, C9_3,   C8_3,   C7_3},
    {1, C9_2,   C8_2,   C7_2},
#else
    // 7 leds
    {1,0,0,0},
    {1,1,1,1},
    {1,2,2,2},
    {1,3,3,3},
    {1,4,4,4},
    {1,5,5,5},
    {1,6,6,6},
#endif

};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3741, 0x60, 0, 0, 97},
#ifdef RABBIT_VER
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 97, 6},
#elif defined(LED_SCREEN_VER)
    {RGB_DRIVER_IS31FL3731, 0xE8, 0, 97, 16},
#else
    {RGB_DRIVER_WS2812,     0, 0, 97, 7},
#endif
//    {RGB_DRIVER_IS31FL3731, 0xE8, 0, 104, 16},
};

rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM] = {
    {0, 0, 97},
};

#ifdef RABBIT_VER
rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {1,  97, 3},
    {2, 100, 3},
};
#elif defined(LED_SCREEN_VER)
rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {1,  97, 12},
    {2, 109, 4},
};
#else
rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {1,  97, 7},
//    {2, 104, 16},
};
#endif

#include "rgb_matrix_stub.h"

led_config_t g_led_config = {
    {
        { 0,   1, 2,       3,      4,      5,      6, 7,       8,     54, 55, 56,     57,     58, 59,     60},
        { 9,  10, 11,     12,     13,     14,     15, 16,     17,     62, 63, 64,     65,     67, 95,     96},
        {18,  19, 20,     21,     22,     23,     24, 25,     26,     71, 72, 73,     74,     70, 94,     61},
        {27,  28, 29,     30,     31,     32,     33, 34,     35,     75, 76, 77, NO_LED,     79, 93,     92},
        {37,  38, 39,     40,     41,     42,     43, 44,     80,     81, 82, 83,     85, NO_LED, 91, NO_LED},
        {45,  46, 47, NO_LED, NO_LED, NO_LED, NO_LED, 49, NO_LED, NO_LED, 51, 52,     53,     88, 89,     90},
    },
    {
           {0,0},   {26,0},   {39,0},   {52,0},   {65,0},   {84,0},   {97,0},  {110,0}, {123,0},
          {0,16},  {13,16},  {26,16},  {39,16},  {52,16},  {65,16},  {78,16},  {91,16}, {104,16},
          {3,28},  {19,28},  {32,28},  {45,28},  {58,28},  {71,28},  {84,28},  {97,28}, {110,28},
          {5,40},  {23,40},  {36,40},  {49,40},  {61,40},  {74,40},  {87,40}, {100,40}, {113,40},
          {2,51},  { 8,52},  {16,51},  {29,52},  {42,52},  {55,52},  {68,52},  {81,52},  {94,52},
          {3,64},  {19,64},  {36,64},  {52,61},  {91,64}, {129,61}, {146,64}, {162,64}, {178,64},
         {142,0},  {155,0},  {168,0},  {181,0},  {198,0},  {211,0},  {224,0},           {224,16},
        {116,16}, {129,16}, {142,16}, {155,16}, {167,13}, {175,16}, {183,13}, {182,29}, {177,27},
        {123,28}, {136,28}, {149,28}, {162,28}, {126,40}, {139,40}, {152,40}, {165,39}, {171,39},
        {107,52}, {120,52}, {133,52}, {146,52}, {162,49}, {170,52}, {178,49}, {181,37},
        {198,64}, {211,64}, {224,64}, {211,52}, {224,28}, {211,28}, {198,28}, {198,16}, {211,16},
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        4, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,

        1, 1, 1, 4, 4, 4, 4, 4, 4,
        1, 1, 4, 4, 4, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1,    1,
        4, 4, 4, 4, 1, 1, 1, 1, 4,

        4, 4, 4, 4, 4, 4, 4, 1, 1,
        4, 4, 4, 4, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    },
};

#ifdef LED_SCREEN_VER
#include <stdlib.h>
#include "rgb_common.h"
#include "rgb_linear.h"

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

void hook_matrix_change_rgb(uint8_t row, uint8_t col, bool pressed)
{
    if (pressed) {
        typing_leds_pressed();
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

#endif

#include "host.h"
#include "led.h"

#define CAPS_LED_INDEX      27
#define SCROLL_LED_INDEX    59

void rgb_led_pre_flush(void)
{
    if (!rgb_matrix_is_enabled()) {
        if (host_keyboard_led_state().caps_lock) {
            rgb_matrix_set_rgb_stub(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
        } else {
            rgb_matrix_set_rgb_stub(0, CAPS_LED_INDEX, 0, 0, 0);
        }
        if (host_keyboard_led_state().scroll_lock) {
            rgb_matrix_set_rgb_stub(0, SCROLL_LED_INDEX, 0xFF, 0xFF, 0xFF);
        } else {
            rgb_matrix_set_rgb_stub(0, SCROLL_LED_INDEX, 0, 0, 0);
        }
    }
#ifdef LED_SCREEN_VER
    if (typing_trigger) {
        typing_leds_flush();
    }
#endif
}
#endif