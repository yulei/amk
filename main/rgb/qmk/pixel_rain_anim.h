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

#ifdef ENABLE_RGB_MATRIX_PIXEL_RAIN
RGB_MATRIX_EFFECT(PIXEL_RAIN)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool PIXEL_RAIN(rgb_matrix_state_t *state) {
    static uint32_t wait_timer = 0;
    if (wait_timer > state->g_rgb_timer) {
        return false;
    }

    inline uint32_t interval(void) { return 500 / scale16by8(qadd8(state->config->speed, 16), 16); }

    bool rain_pixel(uint8_t i, rgb_matrix_state_t *state, bool off) {
        if (!HAS_ANY_FLAGS(g_led_config.flags[i], state->rgb_effect_params.flags)) {
            return true;
        }
        if (off) {
            effect_set_color(state, i, 0, 0, 0);
        } else {
            HSV hsv = {random8(), qadd8(random8() >> 1, 127), state->config->val};
            effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
        }
        wait_timer = state->g_rgb_timer + interval();
        return false;
    }

    return rain_pixel(mod8(random8(), DRIVER_LED_TOTAL), state, random8() & 2);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_PIXEL_RAIN
