/**
 * @file keyboard_display.c
 * @author astro
 * @brief 
 * 
 * @copyright Copyright (c) 2022, astro,  All rights reserved.
 */

#include <string.h>

#include "keyboard_display.h"
#include "matrix.h"
#include "timer.h"
#include "amk_printf.h"

#ifndef KBDDISP_DEBUG
#define KBDDISP_DEBUG 1
#endif

#if KBDDISP_DEBUG
#define kbddisp_debug  amk_printf
#else
#define kbddisp_debug(...)
#endif


typedef struct {
    screen_t        *screen;
    display_t       *display;
    display_param_t param;
    uint8_t         used;
    bool            enabled;
    uint8_t         keyboard;
    uint8_t*        buffer;
    uint32_t        buffer_size;
    uint32_t        ticks;
    bool            dirty;
} keyboard_display_obj_t;

static keyboard_display_obj_t keyboard_display_objs[KEYBOARD_DISPLAY_NUM];

static bool keyboard_display_init(display_t *display, screen_t *screen)
{
    keyboard_display_obj_t *obj = (keyboard_display_obj_t*)display->data;
    obj->screen = screen;

    obj->buffer = screen->get_buffer(screen);
    obj->buffer_size = obj->param.width*obj->param.height*2;

    obj->ticks = timer_read32();
    //obj->dirty = false;
    obj->dirty = true;

    return true;
}

void keyboard_display_uninit(display_t *display)
{
}

void keyboard_display_task(display_t *display)
{
    keyboard_display_obj_t *obj = (keyboard_display_obj_t*)display->data;

    if (!obj->enabled) return;

    if (!obj->screen->ready(obj->screen)) return;

    if (obj->dirty) {
        obj->screen->clear(obj->screen);

        //obj->screen->draw_rect(obj->screen, 0, 0, 10, 10);
        for (int i = 0; i < KEYBOARD_KEY_COUNT; i++) {
            matrix_row_t row = matrix_get_row(keyboard_keys[i].pos.row);
            if (!(row&(1<<keyboard_keys[i].pos.col))) {
                obj->screen->draw_rect(obj->screen, keyboard_keys[i].x+5, keyboard_keys[i].y+20, keyboard_keys[i].w, keyboard_keys[i].h);
            }
        }

        obj->screen->refresh(obj->screen);

        obj->dirty = false;
        obj->ticks = timer_read32();
    }

}

void keyboard_display_set_enable(display_t *display, bool enable)
{
    keyboard_display_obj_t *obj = (keyboard_display_obj_t*)display->data;
    obj->enabled = enable;
}

bool keyboard_display_is_enabled(display_t *display)
{
    keyboard_display_obj_t *obj = (keyboard_display_obj_t*)display->data;

    return obj->enabled;
}

void keyboard_display_matrix_change(display_t *display, keyevent_t event)
{
    keyboard_display_obj_t *obj = (keyboard_display_obj_t*)display->data;

    obj->dirty = true;
}

bool keyboard_display_create(display_t *display, display_param_t *param)
{
    keyboard_display_obj_t *obj = NULL;
    for (int i = 0; i < KEYBOARD_DISPLAY_NUM; i++) {
        if (!keyboard_display_objs[i].used) {
            obj = &keyboard_display_objs[i];
            break;
        }
    }

    if (obj) {
        obj->display = display;
        obj->param = *param;
        obj->used = 1;
        obj->enabled = param->enabled;

        display->data = obj;
        display->init = keyboard_display_init;
        display->uninit = keyboard_display_uninit;
        display->task = keyboard_display_task;
        display->set_enable = keyboard_display_set_enable;
        display->is_enabled = keyboard_display_is_enabled;
        display->matrix_change = keyboard_display_matrix_change;
        return true;
    }

    return false;
}
