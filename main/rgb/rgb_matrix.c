/**
 * @file rgb_matrix.c
 */

#include "rgb_matrix.h"
//#include "rgb_effect_matrix.h"
#include "rgb_effect_matrix_qmk.h"

typedef struct {
    rgb_effect_t effects[RGB_MATRIX_NUM];
} rgb_matrix_t;

static rgb_matrix_t rgb_matrix;

void rgb_matrix_init(void)
{
    // initialize rgb matrix 
    for (int i = 0; i < RGB_MATRIX_NUM; i++) {
        uint8_t config = g_rgb_matrix_params[i].config;
        g_rgb_configs[config].index = config;
        rgb_matrix.effects[i] = rgb_effect_matrix_init(&g_rgb_configs[config], i, g_rgb_matrix_params[i].led_start, g_rgb_matrix_params[i].led_num);
    }
}

void rgb_matrix_task(void)
{
    for (int i = 0; i < RGB_MATRIX_NUM; i++) {
        rgb_effect_matrix_task(rgb_matrix.effects[i]);
    }
}

void rgb_matrix_uninit(void)
{
    for (int i = 0; i < RGB_MATRIX_NUM; i++) {
        rgb_effect_matrix_uninit(rgb_matrix.effects[i]);
    }
}

void rgb_matrix_prepare_sleep(void)
{}