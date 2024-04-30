
#ifdef CUSTOMRGB_ENABLE
RGB_MATRIX_EFFECT(CUSTOMRGB)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

extern void customrgb_effect_begin(void);
extern void customrgb_effect(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val);
extern void customrgb_effect_end(void);

bool CUSTOMRGB(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    customrgb_effect_begin();

    for (uint8_t i = led_min; i < led_max; i++) {
        uint8_t hue = 0;
        uint8_t sat = 0;
        uint8_t val = 0;
        customrgb_effect(i, &hue, &sat, &val);
        HSV hsv = {hue, sat, val};
        RGB rgb       = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    customrgb_effect_end();
    return rgb_matrix_check_finished_leds(led_max);
}
#    endif
#endif