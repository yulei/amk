/**
 * @file rabbit60.c
 */

#include "rabbit60.h"
#include "amk_printf.h"


#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "is31fl3236.h"
#include "is31fl3733.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
#ifdef RGB_MATRIX_ENABLE
    {1, E_1,  D_1,  F_1},
    {1, E_2,  D_2,  F_2},
    {1, E_3,  D_3,  F_3},
    {1, E_4,  D_4,  F_4},
    {1, E_5,  D_5,  F_5},
    {1, E_6,  D_6,  F_6},
    {1, E_7,  D_7,  F_7},
    {1, E_8,  D_8,  F_8},
    {1, E_9,  D_9,  F_9},
    {1, E_10, D_10, F_10},
    {1, E_11, D_11, F_11},
    {1, E_12, D_12, F_12},
    {1, E_13, D_13, F_13},
    {1, E_14, D_14, F_14},
    {1, E_15, D_15, F_15},
    {1, E_16, D_16, F_16},

    {1, B_1,  A_1,  C_1},
    {1, B_2,  A_2,  C_2},
    {1, B_3,  A_3,  C_3},
    {1, B_4,  A_4,  C_4},
    {1, B_5,  A_5,  C_5},
    {1, B_6,  A_6,  C_6},
    {1, B_7,  A_7,  C_7},
    {1, B_8,  A_8,  C_8},
    {1, B_9,  A_9,  C_9},
    {1, B_10, A_10, C_10},
    {1, B_11, A_11, C_11},
    {1, B_12, A_12, C_12},
    {1, B_13, A_13, C_13},
    {1, B_14, A_14, C_14},
    {1, B_15, A_15, C_15},
    {1, B_16, A_16, C_16},

    {1, H_1,  G_1,  I_1},
    {1, H_2,  G_2,  I_2},
    {1, H_3,  G_3,  I_3},
    {1, H_4,  G_4,  I_4},
    {1, H_5,  G_5,  I_5},
    {1, H_6,  G_6,  I_6},
    {1, H_7,  G_7,  I_7},
    {1, H_8,  G_8,  I_8},
    {1, H_9,  G_9,  I_9},
    {1, H_10, G_10, I_10},
    {1, H_11, G_11, I_11},
    {1, H_12, G_12, I_12},
    {1, H_13, G_13, I_13},
    {1, H_14, G_14, I_14},
    {1, H_15, G_15, I_15},
    {1, H_16, G_16, I_16},

    {1, K_1,  J_1,  L_1},
    {1, K_2,  J_2,  L_2},
    {1, K_3,  J_3,  L_3},
    {1, K_4,  J_4,  L_4},
    {1, K_5,  J_5,  L_5},
    {1, K_6,  J_6,  L_6},
    {1, K_7,  J_7,  L_7},
    {1, K_8,  J_8,  L_8},
    {1, K_9,  J_9,  L_9},
    {1, K_10, J_10, L_10},
    {1, K_11, J_11, L_11},
    {1, K_12, J_12, L_12},
    {1, K_13, J_13, L_13},
    {1, K_14, J_14, L_14},
    {1, K_15, J_15, L_15},
    //{1K L_16J K_16L J_16},
#endif
    {0, OUT_22, OUT_23, OUT_24},
    {0, OUT_25, OUT_26, OUT_27},
    {0, OUT_28, OUT_29, OUT_30},

};


#ifdef RGB_MATRIX_ENABLE
rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 63, 3},
    {RGB_DRIVER_IS31FL3733, 0xA0, 0, 0, 63},
};
rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM] = {
    {0, 0, 63},
};
rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {1, 63, 3},
};

#else
rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 3},
};
rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 3},
};
#endif

#ifdef RGB_MATRIX_ENABLE

#include "rgb_matrix_stub.h"

led_config_t g_led_config = {
    {
        {     5,      4,  3,     2,     1,     0,     19, 18,    17, 16,NO_LED,NO_LED, 9,     8, 7, 6},
        {    45,     44, 43,    42,    41,    40,     23, 22,    21, 20,NO_LED,NO_LED,12,    11,47,46},
        {    37,     36, 35,    34,    33,    32,     27, 26,    25, 24,NO_LED,NO_LED,13,NO_LED,39,38},
        {    56,     55, 54,    52,    51,    50,     49, 48,NO_LED, 28,NO_LED,NO_LED,15,    14,58,57},
        {NO_LED, NO_LED, 53,NO_LED,NO_LED,NO_LED, NO_LED, 31,    30, 29,NO_LED,NO_LED,62,    61,60,59},
    },
    {
        {64,0}, {80,0}, {96,0},  {112,0}, {128,0}, {144,0}, {160,0}, {176,0}, {192,0}, {208,0}, {224,0}, {200,16},{220,16},{214,32},{202,48},{224,48},
        {0,0},  {16,0}, {32,0},  {48,0},  {4,16},  {24,16}, {40,16}, {56,16}, {6,32},  {28,32}, {44,32}, {60,32}, {10,48}, {4,64},  {24,64}, {44,64},
        {76,32},{92,32},{108,32},{124,32},{140,32},{156,32},{172,32},{188,32},{72,16}, {88,16}, {104,16},{120,16},{136,16},{152,16},{168,16},{184,16},
        {36,48},{52,48},{68,48}, {84,48}, {100,48},{112,64},{116,48},{132,48},{148,48},{164,48},{180,48},{180,64},{200,64},{220,64}, {216, 0}
    },
    {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
    },
};

#include "host.h"
#include "led.h"

#define CAPS_LED_INDEX 10

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

#endif
