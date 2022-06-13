#ifdef ENABLE_RGB_MATRIX_BAND_SAT
RGB_MATRIX_EFFECT(BAND_SAT)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV BAND_SAT_math(HSV hsv, uint8_t i, uint8_t time) {
    int16_t s = hsv.s - abs(scale8(g_led_config.point[i].x, 228) + 28 - time) * 8;
    hsv.s     = scale8(s < 0 ? 0 : s, hsv.s);
    return hsv;
}

bool BAND_SAT(rgb_matrix_state_t *state) { return effect_runner_i(state, &BAND_SAT_math); }

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_BAND_SAT
