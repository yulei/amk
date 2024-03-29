/* Copyright (C) 2021 @filterpaper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Inspired from 4x12 fractal created by @schwarzgrau

#ifdef ENABLE_RGB_MATRIX_PIXEL_FRACTAL
RGB_MATRIX_EFFECT(PIXEL_FRACTAL)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool PIXEL_FRACTAL(rgb_matrix_state_t *state) {
#        define MID_COL MATRIX_COLS / 2
    static bool led[MATRIX_ROWS][MATRIX_COLS];

    static uint32_t wait_timer = 0;
    if (wait_timer > state->g_rgb_timer) {
        return false;
    }

    inline uint32_t interval(void) { return 3000 / scale16by8(qadd8(state->config->speed, 16), 16); }

    HSV hsv = {state->config->hue, state->config->sat, state->config->val};

    for (uint8_t h = 0; h < MATRIX_ROWS; ++h) {
        for (uint8_t l = 0; l < MID_COL - 1; ++l) {  // Light and move left columns outwards
            if (led[h][l]) {
                effect_set_color(state, g_led_config.matrix_co[h][l], hsv.h, hsv.s, hsv.v);
            } else {
                effect_set_color(state, g_led_config.matrix_co[h][l], 0, 0, 0);
            }
            led[h][l] = led[h][l + 1];
        }

        for (uint8_t r = MATRIX_COLS - 1; r > MID_COL; --r) {  // Light and move right columns outwards
            if (led[h][r]) {
                effect_set_color(state, g_led_config.matrix_co[h][r], hsv.h, hsv.s, hsv.v);
            } else {
                effect_set_color(state, g_led_config.matrix_co[h][r], 0, 0, 0);
            }
            led[h][r] = led[h][r - 1];
        }

        // Light both middle columns
        if (led[h][MID_COL]) {
            effect_set_color(state, g_led_config.matrix_co[h][MID_COL], hsv.h, hsv.s, hsv.v);
        } else{
            effect_set_color(state, g_led_config.matrix_co[h][MID_COL], 0, 0, 0);
        }
        if (led[h][MID_COL - 1]) {
            effect_set_color(state, g_led_config.matrix_co[h][MID_COL - 1], hsv.h, hsv.s, hsv.v);
        } else {
            effect_set_color(state, g_led_config.matrix_co[h][MID_COL - 1], 0, 0, 0);
        }

        // Generate new random fractal columns
        led[h][MID_COL] = led[h][MID_COL - 1] = (random8() & 3) ? false : true;
    }

    wait_timer = state->g_rgb_timer + interval();
    return false;
}
#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_PIXEL_FRACTAL
