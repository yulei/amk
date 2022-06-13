#pragma once

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED

typedef HSV (*reactive_splash_f)(rgb_matrix_state_t *state, HSV hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick);

bool effect_runner_reactive_splash(uint8_t start, rgb_matrix_state_t *state, reactive_splash_f effect_func) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t count = state->g_last_hit_tracker.count;
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        HSV hsv  = {state->config->hue, state->config->sat, state->config->val};
        hsv.v   = 0;
        for (uint8_t j = start; j < count; j++) {
            int16_t  dx   = g_led_config.point[i].x - state->g_last_hit_tracker.x[j];
            int16_t  dy   = g_led_config.point[i].y - state->g_last_hit_tracker.y[j];
            uint8_t  dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = scale16by8(state->g_last_hit_tracker.tick[j], qadd8(state->config->speed, 1));
            hsv           = effect_func(state, hsv, dx, dy, dist, tick);
        }
        hsv.v   = scale8(hsv.v, state->config->val);
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED
