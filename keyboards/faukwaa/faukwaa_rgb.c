/*
 * @file faukwaa_rgb.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "faukwaa.h"
#include "rgb_common.h"
#include "is31fl3236.h"
#include "is31fl3729.h"
#include "rgb_driver.h"
#include "rgb_indicator.h"

struct rgb_indicator g_rgb_indicators[RGB_INDICATOR_NUM] = {
    {74, RGB_INDICATOR_CAPS_LOCK, RGB_INDICATOR_STANDALONE},
};

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // left
    {0,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {0,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {0,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {0,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {0,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {0,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {0,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
//    {4,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
//    {4,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    {0,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {0,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {0,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {0,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {0,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {0,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {0,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
//    {4,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
//    {4,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {0, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {0, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {0, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {0, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {0, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {0, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
//    {4, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
//    {4, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
//    {4, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {0, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {0, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {0, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {0, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {0, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {0, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
//    {4, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
//    {4, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
//    {4, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {0, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {0, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {0, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {0, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {0, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {0, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
//    {4, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
//    {4, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
//    {4, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    // right
    {1, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {1, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {1, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {1, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {1, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {1, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {1, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {1, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {1, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {1, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {1, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {1, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {1, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {1, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {1, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {1, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {1, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {1, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {1, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {1, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {1, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {1, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {1, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {1, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {1, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {1, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
//    {5, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {1,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {1,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {1,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {1,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {1,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {1,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {1,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {1,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
//    {5,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {1,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {1,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {1,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {1,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {1,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {1,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {1,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {1,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
//    {5,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    // left leds
    {2, OUT_25, OUT_26, OUT_27},
    {2, OUT_13, OUT_14, OUT_15},
    {2, OUT_16, OUT_17, OUT_18},
    {2, OUT_19, OUT_20, OUT_21},
    {2, OUT_22, OUT_23, OUT_24},

    // right leds
    {3, OUT_16, OUT_17, OUT_18},
    {3, OUT_19, OUT_20, OUT_21},
    {3, OUT_10, OUT_11, OUT_12},
    {3, OUT_13, OUT_14, OUT_15},

    // middle leds
    {4, OUT_28, OUT_29, OUT_30},
    {4, OUT_4, OUT_5, OUT_6},
    {4, OUT_7, OUT_8, OUT_9},
    {4, OUT_10, OUT_11, OUT_12},

    // bottom leds
    {5, OUT_1, OUT_2, OUT_3},
    {5, OUT_34, OUT_35, OUT_36},
    {5, OUT_4, OUT_5, OUT_6},
    {5, OUT_7, OUT_8, OUT_9},
    {5, OUT_19, OUT_20, OUT_21},
    //{3, OUT_16, OUT_17, OUT_18},
    //{3, OUT_13, OUT_14, OUT_15},
    {5, OUT_10, OUT_11, OUT_12},

};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3729, 0x68, 0, 0, 32},
    {RGB_DRIVER_IS31FL3729, 0x6E, 1,32, 42},
    {RGB_DRIVER_IS31FL3236, 0x78, 0,74, 5},
    {RGB_DRIVER_IS31FL3236, 0x7E, 1,79, 4},
    {RGB_DRIVER_IS31FL3236, 0x7A, 2,83, 4},
    {RGB_DRIVER_IS31FL3236, 0x7C, 3, 87, 6},

};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {1, 75, 4},
    {2, 79, 4},
    {3, 83, 4},
    {4, 87, 4},
    {5, 91, 2},
};

rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM] = {
    {0, 0, 74},
};

#include "rgb_matrix_stub.h"
#define LED_NO

led_config_t g_led_config = {
    {
        {NO_LED, 2,     1,  0,      3,      4,      6,      32,    33,      34,     39,     49,NO_LED,     37, 36,    35},
        {NO_LED, 9,     8,  7,     10,      5,     13,      41,    42,      43,     47,     48,NO_LED,     46, 45,    44},
        {NO_LED,16,    15, 14,     17,     11,     12,      50,    51,      52,     56,     57,NO_LED,     55, 54,    53},
        {NO_LED,22,    21, 20,     23,     18,     19,      58,    59,      60,     64,     73,NO_LED,     63, 62,    61},
        {NO_LED,28,    27, 26,     30,     24,     25,      66,    69,     NO_LED,     71,     72,NO_LED,     68, 65,   NO_LED}
    },
    {
          {0,5}, {22,5}, {37,5}, {52,4}, {67,7},  {82,9}, {97,12},
        {22,18},{41,18},{57,18},{72,20},{87,23},{101,26},{112,15},
        {19,34},{42,31},{57,31},{72,34},{87,36},{102,39},
        {22,44},{46,44},{62,45},{77,48},{91,50},{106,53},
        {13,60},{32,60},{62,58},{66,59},{90,63}, {94,64},

        {115,14},{128,11}, {140,9}, {152,6}, {164,5}, {177,6}, {189,6}, {195,6},{201,6},
        {112,28},{124,25},{136,22},{148,20},{161,18},{173,19},{185,19},{201,19},{224,6},
        {118,40},{130,37},{142,34},{154,32},{166,32},{179,32},{199,32},{224,19},
        {114,53},{126,51},{138,48},{150,45},{163,44},{175,44},{192,44},{224,32},
        {124,64},{127,63},{147,59},{150,58},{199,60},{212,60},{224,60},{212,47},
    },
    {
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,

        4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4,
    },
};
