#ifdef ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
RGB_MATRIX_EFFECT(GRADIENT_LEFT_RIGHT)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool GRADIENT_LEFT_RIGHT(rgb_matrix_state_t *state) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv  = {state->config->hue, state->config->sat, state->config->val};

    uint8_t scale = scale8(64, state->config->speed);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        // The x range will be 0..224, map this to 0..7
        // Relies on hue being 8-bit and wrapping
        hsv.h   = state->config->hue + (scale * g_led_config.point[i].x >> 5);
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
