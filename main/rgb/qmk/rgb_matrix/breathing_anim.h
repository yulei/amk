#ifdef ENABLE_RGB_MATRIX_BREATHING
RGB_MATRIX_EFFECT(BREATHING)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool BREATHING(rgb_matrix_state_t *state) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv  = {state->config->hue, state->config->sat, state->config->val};
    uint16_t time = scale16by8(state->g_rgb_timer, state->config->speed / 8);
    hsv.v         = scale8(abs8(sin8(time) - 128) * 2, hsv.v);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_BREATHING
