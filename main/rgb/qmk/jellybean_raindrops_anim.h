#ifdef ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
RGB_MATRIX_EFFECT(JELLYBEAN_RAINDROPS)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static void jellybean_raindrops_set_color(int i, rgb_matrix_state_t *state) {
    if (!HAS_ANY_FLAGS(g_led_config.flags[i], state->rgb_effect_params.flags)) return;
    HSV hsv = {rand() & 0xFF, qadd8(rand() & 0x7F, 0x80), state->config->val};
    effect_set_color(state, i, hsv.h, hsv.s, hsv.v);

    //RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
    //rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
}

bool JELLYBEAN_RAINDROPS(rgb_matrix_state_t *state) {
    if (!state->rgb_effect_params.init) {
        // Change one LED every tick, make sure speed is not 0
        if (scale16by8(state->g_rgb_timer, qadd8(state->config->speed, 16)) % 5 == 0) {
            jellybean_raindrops_set_color(rand() % DRIVER_LED_TOTAL, state);
        }
        return false;
    }

    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (int i = led_min; i < led_max; i++) {
        jellybean_raindrops_set_color(i, state);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
