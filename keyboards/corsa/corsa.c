/**
 * @file corsa.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "corsa.h"
#include "led.h"
#include "is31fl3236.h"
#include "is31fl3729.h"
#include "rgb_led.h"

#ifdef RGB_MATRIX_ENABLE
#include "rgb_matrix_stub.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // left
    {0, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {0, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {0, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {0, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {0, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {0, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {0, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {0, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {0, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {0, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {0, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {0, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {0, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {0, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {0, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {0, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {0, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {0, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {0, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {0, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {0, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {0, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {0, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {0, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {0, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {0, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {0, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {0,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    //{0,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {0,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {0,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {0,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {0,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {0,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {0,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {0,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    {0,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {0,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {0,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {0,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    //{0,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    //{0,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {0,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    //{0,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {0,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    // right
    {1,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {1,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {1,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {1,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {1,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    //{1,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    //{1,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    //{1,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    //{1,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    {1,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {1,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {1,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {1,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {1,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {1,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    //{1,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    //{1,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    //{1,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {1, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {1, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {1, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {1, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {1, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    //{1, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    //{1, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    //{1, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    //{1, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {1, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {1, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {1, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {1, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {1, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {1, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    //{1, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    //{1, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    //{1, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {1, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {1, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {1, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {1, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {1, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    //{1, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    //{1, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    //{1, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    //{1, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3729, 0x68, 0, 0, 41},
    {RGB_DRIVER_IS31FL3729, 0x6E, 1, 41, 27},
};

rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM] = {
    {0, 0, 68},
};

#include "rgb_matrix_stub.h"
#define LED_NO

led_config_t g_led_config = {
    {
        { 0,     1,  2,      3,      4,      5,  6,     7,       8,     41,     42, 43,     44, NO_LED, 45},
        { 9,    10, 11,     12,     13,     14, 15,    16,      17,     46,     47, 48,     49,     50, 51},
        {18,    19, 20,     21,     22,     23, 24,    25,      26,     52,     53, 54, NO_LED,     55, 56},
        {27,NO_LED, 28,     29,     30,     31, 32,    33,      34,     57,     58, 59,     60,     61, 62},
        {35,    36, 37, NO_LED, NO_LED, NO_LED, 39, NO_LED, NO_LED, NO_LED,     63, 64,     65,     66, 67},
    },
    {
         {3,0}, {17,0}, {32,0}, {47,0}, {62,0}, {76,0},  {91,0}, {106,0}, {121,0},
        {6,16},{25,16},{39,16},{54,16},{69,16},{84,16}, {98,16},{113,16},{128,16},
        {0,31},{28,32},{43,32},{58,32},{73,32},{87,32},{102,32},{117,32},{132,32},
        {12,48},LED_NO {36,48},{51,48},{65,48},{80,48}, {95,48},{110,48},{124,48},
        {4,64},{23,64},{41,64},{58,60},LED_NO  LED_NO   {97,64},LED_NO   {135,60},

         {135,0}, {150,0}, {165,0}, {180,0}, {202,0}, LED_NO  LED_NO LED_NO LED_NO
        {143,16},{158,16},{172,16},{187,16},{206,16},{224,16},LED_NO LED_NO LED_NO
        {146,32},{161,32},{176,32},{200,32},{224,32},LED_NO   LED_NO LED_NO LED_NO
        {139,48},{154,48},{169,48},{189,48},{209,48},{224,48},LED_NO LED_NO LED_NO
        {152,64},{170,64},{194,64},{209,64},{224,64},LED_NO   LED_NO LED_NO LED_NO
    },
    {
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1,    4, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 1,       4,    1,

        4, 4, 4, 4, 1,
        4, 4, 4, 4, 4, 1,
        4, 4, 4, 1, 1,
        4, 4, 4, 1, 1, 1,
        1, 1, 1, 1, 1,
    },
};

#include "host.h"
#include "led.h"

#define CAPS_LED_INDEX 18

#if 1
void rgb_led_pre_flush(void)
{
    if (!rgb_matrix_is_enabled()) {
        if (host_keyboard_led_state().caps_lock) {
            rgb_matrix_set_rgb_stub(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
        } else {
            rgb_matrix_set_rgb_stub(0, CAPS_LED_INDEX, 0, 0, 0);
        }
    }
}
#endif

#else

#include "rgb_linear.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // indicator leds
    {0, OUT_16, OUT_17, OUT_18},  // ESC
    {0, OUT_19, OUT_20, OUT_21},  // CAPS
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 2},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 2},
};

#define CAPS_LED_INDEX  1
#define ESC_LED_INDEX   0

void rgb_led_pre_flush(void)
{
    uint8_t led = host_keyboard_leds();
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
    } else {
        rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0, 0, 0);
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    if(layer_state_is(1)){
        b = 0xFF;
    } 

    if(layer_state_is(2)){
        r = 0xFF;
    } 

    if(layer_state_is(3)){
        g = 0xFF;
    } 
    rgb_linear_set_rgb(0, ESC_LED_INDEX, r, g, b);
}

#endif

