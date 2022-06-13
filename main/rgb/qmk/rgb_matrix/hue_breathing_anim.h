#ifdef ENABLE_RGB_MATRIX_HUE_BREATHING
RGB_MATRIX_EFFECT(HUE_BREATHING)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Change huedelta to adjust range of hue change. 0-255.
// Hue Breathing - All LED's light up
bool HUE_BREATHING(rgb_matrix_state_t *state) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint8_t  huedelta = 12;
    HSV hsv  = {state->config->hue, state->config->sat, state->config->val};

    uint16_t time     = scale16by8(state->g_rgb_timer, state->config->speed / 8);
    hsv.h             = hsv.h + scale8(abs8(sin8(time) - 128) * 2, huedelta);
    //RGB rgb           = hsv_to_rgb(hsv);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        effect_set_color(state, i, hsv.h, hsv.s, hsv.v);

        //rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // DISABLE_RGB_HUE_BREATHING
