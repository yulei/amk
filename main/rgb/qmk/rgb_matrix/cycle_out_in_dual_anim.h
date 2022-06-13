#ifdef ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
RGB_MATRIX_EFFECT(CYCLE_OUT_IN_DUAL)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV CYCLE_OUT_IN_DUAL_math(HSV hsv, int16_t dx, int16_t dy, uint8_t time) {
    dx           = (k_rgb_matrix_center.x / 2) - abs8(dx);
    uint8_t dist = sqrt16(dx * dx + dy * dy);
    hsv.h        = 3 * dist + time;
    return hsv;
}

bool CYCLE_OUT_IN_DUAL(rgb_matrix_state_t *state) { return effect_runner_dx_dy(state, &CYCLE_OUT_IN_DUAL_math); }

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
