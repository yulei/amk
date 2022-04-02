/**
 * @file display.c
 * 
 */

#include "display.h"
#include "anim_display.h"
#include "audio_display.h"

typedef struct {
    screen_t        *screen;
    display_param_t param;
    display_t       display;
    uint8_t         used;
} display_obj_t;

static display_obj_t display_objs[DISPLAY_NUM];

static bool display_init(display_t *display, display_param_t *param)
{
    switch (param->type) {
    case ANIM_DISPLAY:
        return anim_display_create(display, param);
#ifdef AUDIO_ENABLE
    case AUDIO_DISPLAY:
        return audio_display_create(display, param);
#endif
    default:
        break;
    }

    return false;
}

display_t* display_create(display_param_t *param)
{
    display_obj_t *obj = NULL;
    for (int i = 0; i < DISPLAY_NUM; i++) {
        if( !display_objs[i].used) {
            obj = &display_objs[i];
            break;
        }
    }

    if (obj) {
        if (display_init(&obj->display, param)) {
            obj->used = 1;
            obj->param = *param;
            return &obj->display;
        }
    }

    return NULL;
}

void display_destroy(display_t *screen)
{}
