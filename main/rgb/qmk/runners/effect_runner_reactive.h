#pragma once

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED

typedef HSV (*reactive_f)(HSV hsv, uint16_t offset);

bool effect_runner_reactive(rgb_matrix_state_t *state, reactive_f effect_func) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t max_tick = 65535 / qadd8(state->config->speed, 1);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        uint16_t tick = max_tick;
        // Reverse search to find most recent key hit
        for (int8_t j = state->g_last_hit_tracker.count - 1; j >= 0; j--) {
            if (state->g_last_hit_tracker.index[j] == i && state->g_last_hit_tracker.tick[j] < tick) {
                tick = state->g_last_hit_tracker.tick[j];
                break;
            }
        }

        uint16_t offset = scale16by8(tick, qadd8(state->config->speed, 1));
        HSV hsv  = {state->config->hue, state->config->sat, state->config->val};

        hsv = effect_func(hsv, offset);
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED
