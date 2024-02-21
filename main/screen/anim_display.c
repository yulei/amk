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
    ANIM_DISPLAY_NORMAL,        // normal animation
    ANIM_DISPLAY_STATUS,        // display status
    ANIM_DISPLAY_BACKGROUND,    // display background
};

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

extern const uint8_t background_bin[];
extern const uint32_t background_bin_size[];

extern const uint8_t status_bin[];
extern const uint32_t status_bin_size[];

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
    } else if (obj->param.file_type == AST_FILE) {
        obj->anim_type = ANIM_TYPE_STATUS;
    } else if (obj->param.file_type == ABG_FILE) {
        obj->anim_type = ANIM_TYPE_BACKGROUND;
    } else {
        obj->anim_type = ANIM_TYPE_FONT;
    }

    obj->anim_mode = MODE_SEQUENCE;

    if (!anim_mount(true)) {
        ad_debug("failed to mount animation flash\r\n");
        return false;
    }

    obj->anim = anim_open_with_size(NULL, obj->anim_type, obj->param.width, obj->param.height);
    if (!obj->anim) {
        //ad_debug("failed to open animation files: type=%d, widht=%d, height=%d\n", obj->anim_type, obj->param.width, obj->param.height);
        return false;
    }

    return true;
}

void anim_display_uninit(display_t *display)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;

    if (obj->anim) {
        anim_close(obj->anim);
    }
}

static void add_frame(uint32_t width, uint32_t height, uint8_t *buffer, uint16_t color, uint8_t flag)
{
    uint16_t *p = (uint16_t*)buffer;
    if (flag & DISPLAY_FLAGS_EDGE_TOP) {
        for (int x = 0; x < width; x++) {
            p[x] = color;
        }
    }

    if (flag & DISPLAY_FLAGS_EDGE_BOTTOM) {
        for (int x = 0; x < width; x++) {
            p[(height-1)*width + x] = color;
        }
    }

    if (flag & DISPLAY_FLAGS_EDGE_LEFT) {
        for (int y = 0; y < height; y++) {
            p[y*width] = color;
        }
    }
    if (flag & DISPLAY_FLAGS_EDGE_RIGHT) {
        for (int y = 0; y < height; y++) {
            p[y*width+(width-1)] = color;
        }
    }
}

__attribute__((weak))
bool anim_display_need_refresh(uint32_t index)
{
    return true;
}

__attribute__((weak))
uint32_t anim_display_get_current(uint32_t index)
{
    return 0;
}

__attribute__((weak))
void anim_display_post_process(void* buff, uint32_t index)
{}

__attribute__((weak))
void anim_display_get_offset(uint32_t index, uint32_t *x, uint32_t *y)
{}

void anim_display_task(display_t *display)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;

    if (!obj->anim && !(obj->param.flags & DISPLAY_FLAGS_MODE_CUSTOM)) {
        return;
    }

    if (!obj->enabled) return;

    //if (!obj->screen->ready(obj->screen)) return;

    if (!anim_display_need_refresh(obj->param.screen)) return;

    uint32_t x = 0;
    uint32_t y = 0;
    anim_display_get_offset(obj->param.screen, &x, &y);

    if (obj->param.flags & DISPLAY_FLAGS_MODE_CUSTOM) {
        if (obj->anim) {
            uint32_t frame = anim_display_get_current(obj->param.screen);
            if (anim_set_frame(obj->anim, frame) ) {
                anim_step(obj->anim, &obj->delay, obj->buffer, obj->buffer_size);
            }
        }
        anim_display_post_process(obj->buffer, obj->param.screen);
        obj->screen->fill_rect(obj->screen, x, y, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
    } else {
        uint32_t elapsed = timer_elapsed32(obj->ticks);
        //if (obj->param.screen== 2) {
        //    ad_debug("ANIM: elapsed=%d, delay=%d\n", elapsed, obj->delay);
        //}

        if ( false || elapsed > obj->delay) {
            obj->ticks = timer_read32();
            uint32_t step_start = timer_read32();
            uint32_t res = anim_step(obj->anim, &obj->delay, obj->buffer, obj->buffer_size);
            ad_debug("ANIM: Last=%d, Elapsed=%d, Step start=%d, finish=%d\n", obj->ticks, elapsed, step_start, timer_read32());
            if ( 0 == res)  {
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

            if (obj->param.flags) {
                add_frame(obj->param.width, obj->param.height, obj->buffer, 0xFFFF, obj->param.flags);
            }

            //obj->screen->fill_rect_async(obj->screen, 0, 0, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
            anim_display_post_process(obj->buffer, obj->param.screen);
            uint32_t render_start = timer_read32();
            obj->screen->fill_rect(obj->screen, x, y, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
            ad_debug("ANIM: Render start=%d, finish=%d\n", render_start, timer_read32());
            //obj->ticks = timer_read32();
        }
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

void anim_display_update_flags(display_t *display, uint8_t flags)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;
    obj->param.flags = flags;
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
        display->update_flags = anim_display_update_flags;
        display->matrix_change = NULL;
        return true;
    }

    return false;
}

void anim_display_destroy(display_t *display)
{
    anim_display_obj_t *obj = (anim_display_obj_t*)display->data;
    memset(obj, 0, sizeof(anim_display_obj_t));
}

#endif
