/*
 *  @file audio_display.c
 */


#if AUDIO_DISPLAY_NUM 

#include <string.h>

#include "arm_math.h"

#include "audio_display.h"
#include "ring_buffer.h"
#include "timer.h"
#include "amk_printf.h"


#ifndef AUDIO_DEBUG
#define AUDIO_DEBUG 1
#endif

#if AUDIO_DEBUG
#define audio_debug  amk_printf
#else
#define audio_debug(...)
#endif

#define AUDIO_FREQ_SIZE     12

#ifndef AUDIO_DISPLAY_DELAY
#define AUDIO_DISPLAY_DELAY 60
#endif

enum {
    MODE_BAR,
    MODE_MAX
};

typedef struct {
    screen_t        *screen;
    display_t       *display;
    display_param_t param;
    uint8_t         mode;
    uint8_t         used;
    bool            enabled;
    uint8_t*        buffer;
    uint32_t        buffer_size;
    uint32_t        ticks;
    uint32_t        delay;
} audio_display_obj_t;

static audio_display_obj_t audio_display_objs[AUDIO_DISPLAY_NUM];
static uint8_t audio_data[AUDIO_FFT_BUFFER_SIZE];
ring_buffer_t audio_buffer;
#define AUDIO_FRAME_SIZE    ((AUDIO_FFT_SIZE/2)*sizeof(float))

static bool audio_display_init(display_t *display, screen_t *screen)
{
    audio_display_obj_t *obj = (audio_display_obj_t*)display->data;
    obj->screen = screen;

    obj->buffer = screen->get_buffer(screen);
    obj->buffer_size = obj->param.width*obj->param.height*2;

    rb_init(&audio_buffer, audio_data, AUDIO_FFT_BUFFER_SIZE);

    obj->ticks = timer_read32();
    obj->delay = AUDIO_DISPLAY_DELAY;

    return true;
}

void audio_display_uninit(display_t *display)
{
}

// fixed 128 input, 12 output
typedef struct {
    uint16_t first;
    uint16_t last;
} audio_freq_index_t;

const audio_freq_index_t audio_freqs[AUDIO_FREQ_SIZE] = {
    {1,2},
    {2,3},
    {3,4},
    {4,6},
    {6,8},
    {8,10},
    {10,15},
    {15,20},
    {25,35},
    {35,45},
    {45,63},
    {63,128},
};

static void convert_audio_data(float const* src, uint8_t *out)
{
    for (int i = 0; i < AUDIO_FREQ_SIZE; i++) {
        float value = 0.0;
        uint32_t index = 0;

        arm_max_f32(&src[audio_freqs[i].first], audio_freqs[i].last-audio_freqs[i].first, &value, &index);
        if (value > 63) {
            audio_debug("max audio data overflow: %f\r\n", value);
        }

        value *= 2;
        out[i] = value > 128 ? 128: (uint8_t)value;
    }
}

static void draw_audio_rect(uint8_t *buffer, uint32_t x, uint32_t rect_width, uint32_t rect_height, uint32_t width, uint32_t height)
{
    for (int i = 0; i < rect_height; i++) {
        uint8_t *p = buffer + x*2 + i*width*2;
        for (int j = 0; j < rect_width; j++) {
            *p++ = 0xFF;
            *p++ = 0xFF;
        }
    }
}

static void draw_audio_frame(uint8_t *buffer, uint32_t width, uint32_t height, uint8_t *results)
{
    memset(buffer, 0, width*height*2);

    for (int i = 0; i < AUDIO_FREQ_SIZE; i++) {
        draw_audio_rect(buffer, AUDIO_BAR_OFFSET+AUDIO_BAR_WIDTH*i, AUDIO_BAR_WIDTH, results[i], width, height);
    }
}

void audio_display_task(display_t *display)
{
    audio_display_obj_t *obj = (audio_display_obj_t*)display->data;

    if (!obj->enabled) return;

    if (!obj->screen->ready(obj->screen)) return;

    if (rb_used_count(&audio_buffer) < AUDIO_FRAME_SIZE) return;

    static float buffer[AUDIO_FFT_SIZE/2];
    rb_read(&audio_buffer, (uint8_t*)buffer, AUDIO_FRAME_SIZE);
    uint8_t results[12];
    convert_audio_data(buffer, results);

    draw_audio_frame(obj->buffer, obj->param.width, obj->param.height, results);

    uint32_t elapsed = timer_elapsed32(obj->ticks);
    if ( elapsed > obj->delay) {
        obj->screen->fill_rect(obj->screen, 0, 0, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
        obj->ticks = timer_read32();
    }
}

void audio_display_set_enable(display_t *display, bool enable)
{
    audio_display_obj_t *obj = (audio_display_obj_t*)display->data;
    obj->enabled = enable;
}

bool audio_display_is_enabled(display_t *display)
{
    audio_display_obj_t *obj = (audio_display_obj_t*)display->data;

    return obj->enabled;
}

bool audio_display_create(display_t *display, display_param_t *param)
{
    audio_display_obj_t *obj = NULL;
    for (int i = 0; i < ANIM_DISPLAY_NUM; i++) {
        if (!audio_display_objs[i].used) {
            obj = &audio_display_objs[i];
            break;
        }
    }

    if (obj) {
        obj->display = display;
        obj->param = *param;
        obj->used = 1;
        obj->enabled = param->enabled;

        display->data = obj;
        display->init = audio_display_init;
        display->uninit = audio_display_uninit;
        display->task = audio_display_task;
        display->set_enable = audio_display_set_enable;
        display->is_enabled = audio_display_is_enabled;
        display->matrix_change = NULL;
        return true;
    }

    return false;
}

#endif