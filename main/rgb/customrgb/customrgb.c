/**
 * @file customrgb.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
*/

#include "customrgb.h"
#include "rgb_common.h"
#include "rgb_matrix_stub.h"
#include "amk_store.h"

struct amk_rgb_matrix_led_state {
    uint32_t blink_step;
    uint32_t breath_step;
};

extern struct amk_led amk_leds[RGB_LED_NUM];
static struct amk_rgb_matrix_led_state amk_led_states[RGB_LED_NUM];

void customrgb_init(void)
{
    uint32_t start = g_rgb_matrix_params[0].led_start;
    uint32_t count = g_rgb_matrix_params[0].led_num;
    for (int i = 0; i <count; i++) {

        amk_store_get_led(i+start, &amk_leds[i+start]);
        amk_led_states[i].blink_step = 0;
        amk_led_states[i].breath_step = 0;
    }
}

void customrgb_get_matrix_info(uint8_t index, uint8_t* start, uint8_t* count)
{
    if (index < RGB_MATRIX_NUM) {
        *start = g_rgb_matrix_params[index].led_start;
        *count = g_rgb_matrix_params[index].led_num;
    }
}

uint8_t customrgb_get_matrix_mode(uint8_t index, uint8_t* custom, uint8_t* count)
{
    if (index < RGB_MATRIX_NUM) {
        *custom = RGB_MATRIX_CUSTOM_CUSTOMRGB;
        *count =  RGB_MATRIX_EFFECT_MAX;
    }
    return rgb_matrix_get_mode();
}

uint8_t customrgb_get_matrix_led_index(uint8_t index, uint8_t row, uint8_t col)
{
    if ((index < RGB_MATRIX_NUM) && (row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
        return g_led_config.matrix_co[row][col];
    }
    return NO_LED;
}

void customrgb_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param)
{
    if (index < RGB_LED_NUM) {
        *hue = amk_leds[index].hue;
        *sat = amk_leds[index].sat;
        *val = amk_leds[index].val;
        *param = amk_leds[index].param;
    }
}

void customrgb_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param)
{
    if (index < RGB_LED_NUM) {
        bool changed = false;
        if (amk_leds[index].hue != hue) {
            amk_leds[index].hue = hue;
            changed = true;
        }
        if (amk_leds[index].sat != sat) {
            amk_leds[index].sat = sat;
            changed = true;
        }
        if (amk_leds[index].val != val) {
            amk_leds[index].val = val;
            changed = true;
        }
        if (amk_leds[index].param != param) {
            amk_leds[index].param = param;
            changed = true;
        }
        if (changed) {
            amk_store_set_led(index, &amk_leds[index]);
        }
    }
}

void customrgb_effect_begin(void)
{}

extern const uint8_t rgblight_effect_breathe_table[];

static uint32_t custom_speed = 0;

void customrgb_effect(uint8_t index, uint8_t* h, uint8_t* s, uint8_t* v)
{
    uint8_t start = g_rgb_matrix_params[0].led_start;

    struct amk_led *led = &amk_leds[index + start];
    struct amk_rgb_matrix_led_state *led_state = &amk_led_states[index + start];
    uint32_t speed = led->speed + 1;

    if (led->on) {
        uint8_t hue = led->hue;
        uint8_t sat = led->sat;
        uint8_t val = led->val;
        if ((custom_speed%speed) == 0) {
            // do effect
            if (led->breath) {
                uint8_t breath_index = (led_state->breath_step) % 256;
                val = rgblight_effect_breathe_table[breath_index];
                led_state->breath_step += 16;
            }

            if (led->blink) {
                if(led_state->blink_step % 2 == 0) {
                    hue = 0;
                    sat = 0;
                    val = 0;
                }
                led_state->blink_step++;
            }

            if (led->dynamic) {
                led->hue += HUE_STEP;
            }

        }
        *h = hue;
        *s = sat;
        *v = val;
    } else {
        *h = 0;
        *s = 0;
        *v = 0;
    }

}

void customrgb_effect_end(void)
{
    custom_speed ++;
}