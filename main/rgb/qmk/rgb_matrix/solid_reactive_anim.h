#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
#    ifdef ENABLE_RGB_MATRIX_SOLID_REACTIVE
RGB_MATRIX_EFFECT(SOLID_REACTIVE)
#        ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV SOLID_REACTIVE_math(HSV hsv, uint16_t offset) {
    hsv.h += qsub8(130, offset);
    return hsv;
}

bool SOLID_REACTIVE(rgb_matrix_state_t *state) { return effect_runner_reactive(state, &SOLID_REACTIVE_math); }

#        endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#    endif      // ENABLE_RGB_MATRIX_SOLID_REACTIVE
#endif          // RGB_MATRIX_KEYREACTIVE_ENABLED
