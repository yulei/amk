#ifdef ENABLE_RGB_MATRIX_RAINDROPS
RGB_MATRIX_EFFECT(RAINDROPS)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static void raindrops_set_color(int i, rgb_matrix_state_t *state) {
    if (!HAS_ANY_FLAGS(g_led_config.flags[i], state->rgb_effect_params.flags)) return;
    HSV hsv = {0, state->config->sat, state->config->val};

    // Take the shortest path between hues
    int16_t deltaH = ((state->config->hue + 180) % 360 - state->config->hue) / 4;
    if (deltaH > 127) {
        deltaH -= 256;
    } else if (deltaH < -127) {
        deltaH += 256;
    }

    hsv.h   = state->config->hue + (deltaH * (rand() & 0x03));
    effect_set_color(state, i, hsv.h, hsv.s, hsv.v);
}

bool RAINDROPS(rgb_matrix_state_t *state) {
    if (!state->rgb_effect_params.init) {
        // Change one LED every tick, make sure speed is not 0
        if (scale16by8(state->g_rgb_timer, qadd8(state->config->speed, 16)) % 10 == 0) {
            raindrops_set_color(rand() % DRIVER_LED_TOTAL, state);
        }
        return false;
    }

    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (int i = led_min; i < led_max; i++) {
        raindrops_set_color(i, state);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_RAINDROPS
