/*
 * pad.c
 */

#include "pad.h"
#include "is31fl3731.h"

rgb_led_t g_is31_leds[] ={
    {0, C1_9, C3_10, C4_10},
    {0, C1_10, C2_10, C4_11},
    {0, C1_11, C2_11, C3_11},
    {0, C1_12, C2_12, C3_12},
    {0, C1_13, C2_13, C3_13},
    {0, C1_14, C2_14, C3_14},

    {0, C9_9, C8_9, C7_9},
    {0, C9_10, C8_10, C7_10},
    {0, C9_11, C8_11, C7_11},
    {0, C9_12, C8_12, C7_12},
    {0, C9_13, C8_13, C7_13},
    {0, C9_14, C8_14, C7_14},
    {0, C9_15, C8_15, C6_14},

    {0, C1_1, C3_2, C4_2},
    {0, C1_2, C2_2, C4_3},
    {0, C1_3, C2_3, C3_3},
    {0, C1_4, C2_4, C3_4},
    {0, C1_5, C2_5, C3_5},
    {0, C1_6, C2_6, C3_6},
    {0, C1_7, C2_7, C3_7},
    {0, C1_8, C2_8, C3_8},

    {0, C9_1, C8_1, C7_1},
    {0, C9_2, C8_2, C7_2},
    {0, C9_3, C8_3, C7_3},
    {0, C9_4, C8_4, C7_4},
    {0, C9_5, C8_5, C7_5},
};

rgb_matrix_t g_rgb_matrix = {
    .attributes = {
        {172,  0,0,0,2},
        {255,  3,0,0,3},
        {255,  0,0,0,3},
        {255, 63,0,1,3},
        {172, 63,0,1,2},
        { 89, 63,0,1,1},

        { 89,111,0,2,1},
        {172,111,0,2,2},
        {255, 85,0,2,3},
        {255,159,0,3,3},
        {255,159,0,3,3},
        {172,159,0,3,2},
        { 89,159,0,3,1},

        {  5, 63,0,1,0},
        {  5,111,0,2,0},
        {  5,159,0,3,0},
        {  5,208,0,4,0},
        { 89,208,0,4,1},
        {172,208,0,4,2},
        {255,208,0,4,3},
        {255,222,0,5,3},

        {  0,252,0,5,0},
        { 95,252,0,5,1},
        { 95,255,0,5,1},
        {172,255,0,5,2},
        {255,255,0,5,3},
    },
};


#ifdef RGB_MATRIX_ENABLE
#include "action.h"
#include "action_layer.h"
bool hook_process_action_main(keyrecord_t *record)
{
    if (IS_NOEVENT(record->event) || !record->event.pressed) { 
        return false;
    }
    action_t action = layer_switch_get_action(record->event);
    if (action.kind.id != ACT_MODS) {
        return false;
    }

    switch(action.key.code) {
        case KC_F13:
            rgb_matrix_toggle();
            amk_printf("Toggle rgb matrix: %d\n", rgb_matrix_enabled());
            return true;
        case KC_F14:
            rgb_matrix_inc_mode();
            return true;
        case KC_F15:
            rgb_matrix_inc_hue();
            return true;
        case KC_F16:
            rgb_matrix_dec_hue();
            return true;
        case KC_F17:
            rgb_matrix_inc_sat();
            return true;
        case KC_F18:
            rgb_matrix_dec_sat();
            return true;
        case KC_F19:
            rgb_matrix_inc_val();
            return true;
        case KC_F20:
            rgb_matrix_dec_val();
            return true;
        case KC_F21:
            rgb_matrix_inc_speed();
            return true;
        case KC_F22:
            rgb_matrix_dec_speed();
            return true;
        default:
            break;
    }

    return false;
}
#endif