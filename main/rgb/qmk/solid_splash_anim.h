#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
#    if defined(ENABLE_RGB_MATRIX_SOLID_SPLASH) || defined(ENABLE_RGB_MATRIX_SOLID_MULTISPLASH)

#        ifdef ENABLE_RGB_MATRIX_SOLID_SPLASH
RGB_MATRIX_EFFECT(SOLID_SPLASH)
#        endif

#        ifdef ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
RGB_MATRIX_EFFECT(SOLID_MULTISPLASH)
#        endif

#        ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

HSV SOLID_SPLASH_math(rgb_matrix_state_t *state, HSV hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
    uint16_t effect = tick - dist;
    if (effect > 255) effect = 255;
    hsv.v = qadd8(hsv.v, 255 - effect);
    return hsv;
}

#            ifdef ENABLE_RGB_MATRIX_SOLID_SPLASH
bool SOLID_SPLASH(rgb_matrix_state_t *state) { return effect_runner_reactive_splash(qsub8(g_last_hit_tracker.count, 1), state, &SOLID_SPLASH_math); }
#            endif

#            ifdef ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
bool SOLID_MULTISPLASH(rgb_matrix_state_t *state) { return effect_runner_reactive_splash(0, state, &SOLID_SPLASH_math); }
#            endif

#        endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#    endif      // !defined(ENABLE_RGB_MATRIX_SPLASH) && !defined(ENABLE_RGB_MATRIX_MULTISPLASH)
#endif          // RGB_MATRIX_KEYREACTIVE_ENABLED
