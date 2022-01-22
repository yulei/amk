#pragma once

typedef HSV (*i_f)(HSV hsv, uint8_t i, uint8_t time);

bool effect_runner_i(rgb_matrix_state_t *state, i_f effect_func) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t time = scale16by8(state->g_rgb_timer, qadd8(state->config->speed / 4, 1));
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        HSV hsv = {state->config->hue, state->config->sat, state->config->val};
        hsv = effect_func(hsv, i, time);

        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}
