/**
 * @file display.c
 * 
 */

#include "display.h"
#if ANIM_DISPLAY_NUM
#include "anim_display.h"
#endif
#if AUDIO_DISPLAY_NUM
#include "audio_display.h"
#endif
#if KEYBOARD_DISPLAY_NUM
#include "keyboard_display.h"
#endif
#if INFO_DISPLAY_NUM
#include "info_display.h"
#endif
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
#if ANIM_DISPLAY_NUM
    case ANIM_DISPLAY:
        return anim_display_create(display, param);
#endif
#if AUDIO_DISPLAY_NUM
    case AUDIO_DISPLAY:
        return audio_display_create(display, param);
#endif
#if KEYBOARD_DISPLAY_NUM
    case KEYBOARD_DISPLAY:
        return keyboard_display_create(display, param);
#endif
#if INFO_DISPLAY_NUM
    case INFO_DISPLAY:
        return info_display_create(display, param);
#endif
    default:
        break;
    }

    return false;
}

static void display_uninit(display_t *display)
{
    display_obj_t *obj = (display_obj_t*)display->data;

    switch (obj->param.type) {
#if ANIM_DISPLAY_NUM
    case ANIM_DISPLAY:
        anim_display_destroy(display);
        break;
#endif
#if AUDIO_DISPLAY_NUM
    case AUDIO_DISPLAY:
        audio_display_destroy(display);
        break;
#endif
#if KEYBOARD_DISPLAY_NUM
    case KEYBOARD_DISPLAY:
        keyboard_display_destroy(display);
        break;
#endif
#if INFO_DISPLAY_NUM
    case INFO_DISPLAY:
        info_display_destroy(display);
        break;
#endif
    default:
        break;
    }
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

void display_destroy(display_t *display)
{
    display_obj_t *obj = (display_obj_t*)display->data;

    display_uninit(display);

    memset(obj, 0, sizeof(display_obj_t));
}
