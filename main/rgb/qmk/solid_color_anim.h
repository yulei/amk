RGB_MATRIX_EFFECT(SOLID_COLOR)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool SOLID_COLOR(rgb_matrix_state_t *state) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv = {state->config->hue, state->config->sat, state->config->val};
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
        //rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
