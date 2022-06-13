#ifdef ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
RGB_MATRIX_EFFECT(GRADIENT_UP_DOWN)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool GRADIENT_UP_DOWN(rgb_matrix_state_t *state) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv  = {state->config->hue, state->config->sat, state->config->val};

    uint8_t scale = scale8(64, state->config->speed);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        // The y range will be 0..64, map this to 0..4
        // Relies on hue being 8-bit and wrapping
        hsv.h   = state->config->hue + scale * (g_led_config.point[i].y >> 4);
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
