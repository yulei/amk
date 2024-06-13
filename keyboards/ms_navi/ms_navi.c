/**
 * @file ms_navi.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "ms_navi.h"

#include "is31fl3236.h"
#include "is31fl3729.h"
#include "rgb_led.h"

#ifdef RGB_ENABLE

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, OUT_36, OUT_35, OUT_34},
    {0, OUT_33, OUT_32, OUT_31},
    {0, OUT_30, OUT_29, OUT_28},
//    {0, OUT_27, OUT_26, OUT_25},

#ifdef RGB_MATRIX_ENABLE
// left
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
    {1, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},
 
    {1,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {1,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {1,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {1,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {1,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {1,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {1,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {1,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {1,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

//    {1,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
//    {1,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
//    {1,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {1,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
//    {1,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {1,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {1,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {1,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {1,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    // right
    {2, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {2, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {2, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {2, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {2, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {2, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {2, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {2, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {2, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {2, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {2, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    //{2, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    //{2, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {2, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {2, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {2, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {2, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {2, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    //{2, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    //{2, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    //{2, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    //{2, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    //{2, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {2, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {2, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {2, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {2, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},


    //{2,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    //{2,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    //{2,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {2,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {2,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {2,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {2,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {2,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {2,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {2,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    //{2,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    //{2,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {2,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {2,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {2,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {2,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {2,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {2,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},
#endif
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 3},
#ifdef RGB_MATRIX_ENABLE
    {RGB_DRIVER_IS31FL3729, 0x68, 0, 3, 41},
    {RGB_DRIVER_IS31FL3729, 0x6E, 1, 44, 33},
#endif
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 3},
//    {1, 3, 1},
};


#ifdef RGB_MATRIX_ENABLE
rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM] = {
    {1, 3, 74},
};

#include "rgb_matrix_stub.h"
led_config_t g_led_config = {
    {
        {    17,    16,    15,    14,    13,   12,   11,    10,     5,     4,     3,    2,    1,     8,     7,    6,},
        {    55,    54,    53,    52,    50,   41,   42,    51,     0,    49,    48,   47,   46,    45,    44,   43,},
        {    31,    30,    29,    28,    36,   27,   66,    65,NO_LED,    60,    59,   58,   57,    56,     9,   18,},
        {    35,    40,    39,    34,    33,   38,   37,    32,    22,    21,    20,   19,   26,    25,    24,   23,},
        {NO_LED,NO_LED,NO_LED,NO_LED,    69,   68,   67,    61,    64,    63,    62,   72,   73,    71,    70,NO_LED,}, 
    },
    {
        {103,0},{91,0},{78,0},{65,0},{52,0},{39,0},{25,0},{13,0},{0,0},
        {110,16},{97,16},{84,16},{71,16},{58,16},{45,16},{32,16},{19,16},{3,16},
        {114,32},{101,32},{88,32},{75,32},{62,32},{49,32},{36,32},{23,32},{3,32},
        {120,48},{107,48},{94,48},{81,48},{68,48},{55,48},{42,48},{29,48},{8,48},
        {91,64},{42,64},{36,64},{18,64},{2,64},

        {211,0},{198,0},{182,0},{175,0},{169,0},{156,0},{143,0},{130,0},{117,0},
        {224,0},{198,16},{178,16},{162,16},{149,16},{136,16},{123,16},
        {174,32},{152,32},{140,32},{127,32},
        {211,48},{182,48},{170,48},{164,48},{146,48},{133,48},
        {224,64},{211,64},{198,64},{179,64},{164,64},{146,64},{140,64},
    },
    {
        4, 4, 4, 4, 4, 4, 4, 4, 1,
        4, 4, 4, 4, 4, 4, 4, 4, 1,
        4, 4, 4, 4, 4, 4, 4, 4, 1,
        4, 4, 4, 4, 4, 4, 4, 4, 1,
                 4,    1, 1, 1, 1,

        1, 1, 4, 1, 4, 4, 4, 4, 4,
        1, 1,       4, 4, 4, 4, 4,
                       1, 4, 4, 4,
                 1, 1, 1, 1, 4, 4,
        1,       1, 1, 1, 1, 1, 1,
    },
};
#endif

#endif

#if APC_SPECIAL_KEY_NUM
#ifdef RGB_MATRIX_ENABLE
#include "amk_apc.h"
struct apc_special_key apc_special_keys[APC_SPECIAL_KEY_NUM] = {
    {3, 12, 1200, 1500, 300, 100}, // caps
};
#else
struct apc_special_key apc_special_keys[APC_SPECIAL_KEY_NUM] = {
    {3, 12, 2250, 2550, 300, 100}, // caps
};
#endif
#endif