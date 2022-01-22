#ifdef ENABLE_RGB_MATRIX_ALPHAS_MODS
RGB_MATRIX_EFFECT(ALPHAS_MODS)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// alphas = color1, mods = color2
bool ALPHAS_MODS(rgb_matrix_state_t *state) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv1  = {state->config->hue, state->config->sat, state->config->val};
    HSV hsv2  = hsv1;
    hsv2.h += state->config->speed;

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_MODIFIER)) {
            effect_set_color(state, i, hsv2.h, hsv2.s, hsv2.v);
        } else {
            effect_set_color(state, i, hsv1.h, hsv1.s, hsv1.v);
        }
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_ALPHAS_MODS
