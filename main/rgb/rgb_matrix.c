/**
 * rgb_matrix.c
 */

/**
 * 32x128 rgb led matrix should be enough for keyboards
 *  keycaps width was based on 0.25u
 */

#include "rgb_color.h"

#define RGB_ROW_UNIT    15
#define RGB_COL_UNIT    5

#define RGB_MATRIX_WIDTH    (RGB_ROW_UNIT*4)
#define RGB_MATRIX_HEIGHT   (RGB_COL_UNIT*2)

void rgb_matrix_init(rgb_driver_t* driver)
{}

bool rgb_matrix_enabled(void)
{}

void rgb_matrix_inc_hue(void)
{}
void rgb_matrix_dec_hue(void)
{}
void rgb_matrix_inc_sat(void)
{}
void rgb_matrix_dec_sat(void)
{}
void rgb_matrix_inc_val(void)
{}
void rgb_matrix_dec_val(void)
{}
void rgb_matrix_inc_speed(void)
{}
void rgb_matrix_dec_speed(void)
{}
void rgb_matrix_inc_mode(void)
{}
void rgb_matrix_dec_mode(void)
{}
void rgb_matrix_toggle(void)
{}
void rgb_matrix_task(void)
{}