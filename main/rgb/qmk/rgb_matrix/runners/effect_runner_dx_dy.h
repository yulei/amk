#pragma once

typedef HSV (*dx_dy_f)(HSV hsv, int16_t dx, int16_t dy, uint8_t time);

bool effect_runner_dx_dy(rgb_matrix_state_t *state, dx_dy_f effect_func) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t time = scale16by8(state->g_rgb_timer, state->config->speed / 2);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        int16_t dx  = g_led_config.point[i].x - k_rgb_matrix_center.x;
        int16_t dy  = g_led_config.point[i].y - k_rgb_matrix_center.y;
        HSV     hsv = {state->config->hue, state->config->sat, state->config->val};
        hsv = effect_func(hsv, dx, dy, time);
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}
