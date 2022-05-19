/*
 *  @file anim_display.c
 */

#if ANIM_DISPLAY_NUM

#include <string.h>

#include "anim_display.h"
#include "timer.h"
#include "wait.h"
#include "anim.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef AD_DEBUG
#define AD_DEBUG 1
#endif

#if AD_DEBUG
#define ad_debug  amk_printf
#else
#define ad_debug(...)
#endif

enum {
    MODE_SINGLE,
    MODE_SEQUENCE,
    MODE_RANDOM,
    MODE_MAX
};

typedef struct {
    screen_t        *screen;
    display_t       *display;
    display_param_t param;
    anim_t          *anim;
    uint8_t         *buffer;
    uint32_t        buffer_size;
    uint8_t         anim_type;
    uint8_t         anim_mode;
    uint8_t         used;
    bool            enabled;
    uint32_t        ticks;
    uint32_t        delay;
} anim_display_obj_t;

static anim_display_obj_t anim_display_objs[ANIM_DISPLAY_NUM];

static bool anim_display_init(display_t *display, screen_t *screen)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;
    obj->screen = screen;
    obj->buffer_size = obj->param.width*obj->param.height*2;
    obj->buffer = screen->get_buffer(screen);
    memset(obj->buffer, 0, obj->buffer_size);
    obj->ticks = timer_read32();
    obj->delay = 0;

    if (obj->param.file_type == ANM_FILE) {
        obj->anim_type = ANIM_TYPE_MAIN;
    } else if (obj->param.file_type == AUX_FILE) {
        obj->anim_type = ANIM_TYPE_AUX;
    } else {
        obj->anim_type = ANIM_TYPE_FONT;
    }

    obj->anim_mode = MODE_SEQUENCE;

    if (!anim_mount(true)) {
        ad_debug("failed to mount animation flash\r\n");
        return false;
    }

    obj->anim = anim_open(NULL, obj->anim_type);
    if (!obj->anim) {
        return false;
    }

    return true;
}

void anim_display_uninit(display_t *display)
{
}

static void add_frame(uint32_t width, uint32_t height, uint8_t *buffer, uint16_t color)
{
    uint16_t *p = (uint16_t*)buffer;
    for (int x = 0; x < width; x++) {
        p[x] = color;
        p[(height-1)*width + x] = color;
    }

    for (int y = 0; y < height; y++) {
        p[y*width] = color;
        p[y*width+(width-1)] = color;
    }
}

void anim_display_task(display_t *display)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;

    if (!obj->anim) return;

    if (!obj->enabled) return;

    //if (!obj->screen->ready(obj->screen)) return;

    uint32_t elapsed = timer_elapsed32(obj->ticks);
    if ( elapsed > obj->delay) {
        if ( 0 == anim_step(obj->anim, &obj->delay, obj->buffer, obj->buffer_size)) {
            bool play = false;
            switch(obj->anim_mode) {
                case MODE_SINGLE:
                    if (anim_rewind(obj->anim)) {
                        play = true;
                    }
                    break;
                case MODE_SEQUENCE:
                    if (anim_next(obj->anim)) {
                        play = true;
                    }
                    break;
                case MODE_RANDOM:
                    if (anim_random(obj->anim)) {
                        play = true;
                    }
                    break;
                default:
                    break;
            }
            if (play) {
                anim_step(obj->anim, &obj->delay, obj->buffer, obj->buffer_size);
            } else {
                //anim_close(render->anim);
                //render->anim = NULL;
            }
        }
        if (obj->param.flags&DISPLAY_FLAGS_FRAME) {
            add_frame(obj->param.width, obj->param.height, obj->buffer, 0xFFFF);
        }

        //obj->screen->fill_rect_async(obj->screen, 0, 0, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
        obj->screen->fill_rect(obj->screen, 0, 0, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);

        obj->ticks = timer_read32();
    }

}

void anim_display_set_enable(display_t *display, bool enable)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;
    obj->enabled = enable;
}

bool anim_display_is_enabled(display_t *display)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;

    return obj->enabled;
}

bool anim_display_create(display_t *display, display_param_t *param)
{
    anim_display_obj_t *obj = NULL;
    for (int i = 0; i < ANIM_DISPLAY_NUM; i++) {
        if (!anim_display_objs[i].used) {
            obj = &anim_display_objs[i];
            break;
        }
    }

    if (obj) {
        obj->display = display;
        obj->param = *param;
        obj->used = 1;
        obj->enabled = param->enabled;

        display->data = obj;
        display->init = anim_display_init;
        display->uninit = anim_display_uninit;
        display->task = anim_display_task;
        display->set_enable = anim_display_set_enable;
        display->is_enabled = anim_display_is_enabled;
        display->matrix_change = NULL;
        return true;
    }

    return false;
}

#endif
