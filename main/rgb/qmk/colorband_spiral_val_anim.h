#ifdef ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
RGB_MATRIX_EFFECT(BAND_SPIRAL_VAL)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV BAND_SPIRAL_VAL_math(HSV hsv, int16_t dx, int16_t dy, uint8_t dist, uint8_t time) {
    hsv.v = scale8(hsv.v + dist - time - atan2_8(dy, dx), hsv.v);
    return hsv;
}

bool BAND_SPIRAL_VAL(rgb_matrix_state_t *state) { return effect_runner_dx_dy_dist(state, &BAND_SPIRAL_VAL_math); }

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
