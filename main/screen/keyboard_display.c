/**
 * @file keyboard_display.c
 * @author astro
 * @brief 
 * 
 * @copyright Copyright (c) 2022, astro,  All rights reserved.
 */

#if KEYBOARD_DISPLAY_NUM 

#include <string.h>
#include <stdlib.h>
#include <math.h>

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

#define FRAME_INTERVAL          100
#define FRAME_WIDTH             60
#define FRAME_HEIGHT            20
#define POINT_SPEED_MAX         10
#define POINT_SPEED_MIN         1
#define POINT_ANGLE_STEP        16

#define KEY_POINT_COUNT         10
#define KEY_ANIMATION_COUNT     40
#define KEY_FRAME_SIZE          20

#define ECG_POINT_COUNT         4
#define ECG_ANIMATION_COUNT     10
#define ECG_WIDTH_MIN           5
#define ECG_WIDTH_MAX           7

enum {
    KEYHIT_FWK,
    KEYHIT_ECG,
};

enum {
    KA_IDLE,
    KA_RISE,
    KA_BOOM,
};

typedef struct {
    uint8_t x;
    uint8_t y;
} ecg_point_t;

typedef struct {
    ecg_point_t points[ECG_POINT_COUNT];
    uint8_t current_x;
    uint8_t width;
} ecg_t;

typedef struct {
    ecg_t   ecg[ECG_ANIMATION_COUNT];
    uint8_t start;
    uint8_t current;
} ecg_animation_t;

typedef struct {
    float   x;
    float   y;
    float   speed;
    float   angle;
} keyhit_point_t;

typedef struct {
    keyhit_point_t points[KEY_POINT_COUNT];
    uint8_t center_x;
    uint8_t center_y;
    uint8_t current_y;
    uint8_t state;
} keyhit_animation_t;

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

static keyhit_animation_t  keyhits[KEY_ANIMATION_COUNT];
static uint8_t keyhits_index = 0;

//static ecg_animation_t ecg;
//static uint8_t keyhits_mode = KEYHIT_FWK;

static void keyhit_init(void)
{
    srand(timer_read32());

    for (int i = 0; i < KEY_ANIMATION_COUNT; i++) {
        keyhit_animation_t *kha = &keyhits[i];
        kha->center_x = 0;
        kha->center_y = 0;
        kha->current_y = 0;
        kha->state = KA_IDLE;
        for (int j = 0; j < KEY_POINT_COUNT; j++) {
            kha->points[j].x = 0.0;
            kha->points[j].y = 0.0;
            kha->points[j].speed = 0.0;
            kha->points[j].angle = 0.0;
        }
    }
}

static bool keyhit_point_valid(keyhit_point_t *point, uint8_t center_x, uint8_t center_y)
{
    if (!(point->x < FRAME_WIDTH) || !(point->x > 0.0)) return false;
    if (!(point->y < FRAME_HEIGHT) || !(point->y > 0.0)) return false;

    if (point->x > (center_x+KEY_FRAME_SIZE)) return false;
    if (point->x < (center_x-KEY_FRAME_SIZE)) return false;

    if (point->y > (center_y+KEY_FRAME_SIZE)) return false;
    if (point->y < (center_y-KEY_FRAME_SIZE)) return false;

    return true;
}

static void keyhit_point_update(bool hit)
{
    if (hit) {
        keyhit_animation_t *kha = &keyhits[keyhits_index];
        kha->state = KA_RISE;
        kha->center_x = rand() % FRAME_WIDTH;
        kha->center_y = rand() % FRAME_HEIGHT;
        kha->current_y = 0;

        for (int i = 0; i< KEY_POINT_COUNT; i++) {
            keyhit_point_t *pt = &kha->points[i];
            pt->x = kha->center_x;
            pt->y = kha->center_y;
            pt->speed = rand() % POINT_SPEED_MAX;
            if (pt->speed < 1.0f) pt->speed = 1.0f;
            pt->angle = i*POINT_ANGLE_STEP;
        }
        keyhits_index = (keyhits_index+1) % KEY_ANIMATION_COUNT;
    } else {
        for (int i = 0; i < KEY_ANIMATION_COUNT; i++) {
            keyhit_animation_t *kha = &keyhits[i];
            switch(kha->state) {
            case KA_RISE:
                if (kha->current_y < kha->center_y) {
                    kha->current_y++;
                } else {
                    kha->state = KA_BOOM;
                }
                break;
            case KA_BOOM: {
                bool done = true;
                for(int j = 0; j < KEY_POINT_COUNT; j++) {
                    keyhit_point_t *pt = &kha->points[j];
                    if (keyhit_point_valid(pt, kha->center_x, kha->center_y)) {
                        pt->x += pt->speed * sin((pt->angle/(POINT_ANGLE_STEP*(KEY_POINT_COUNT+1)*1.0)) * 2.0 * M_PI);
                        pt->y += pt->speed * cos((pt->angle/(POINT_ANGLE_STEP*(KEY_POINT_COUNT+1)*1.0)) * 2.0 * M_PI);
                        done = false;
                    }
                }
                if (done) {
                    kha->state = KA_IDLE;
                }
                }break;
            case KA_IDLE:
            default:
                break;
            }
        }
    }
}

static bool keyboard_display_init(display_t *display, screen_t *screen)
{
    keyboard_display_obj_t *obj = (keyboard_display_obj_t*)display->data;
    obj->screen = screen;

    obj->buffer = screen->get_buffer(screen);
    obj->buffer_size = obj->param.width*obj->param.height*2;

    obj->ticks = timer_read32();
    //obj->dirty = false;
    obj->dirty = true;

    keyhit_init();
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

    bool pt_update = (timer_elapsed32(obj->ticks) > FRAME_INTERVAL) ? true : false;
    if (obj->dirty || pt_update) {
        obj->screen->clear(obj->screen);

        //obj->screen->draw_rect(obj->screen, 0, 0, 10, 10);
        for (int i = 0; i < KEYBOARD_KEY_COUNT; i++) {
            matrix_row_t row = matrix_get_row(keyboard_keys[i].pos.row);
            if (!(row&(1<<keyboard_keys[i].pos.col))) {
                obj->screen->draw_rect(obj->screen, keyboard_keys[i].x, keyboard_keys[i].y, keyboard_keys[i].w, keyboard_keys[i].h);
            }
        }

        for (int i = 0; i < KEY_ANIMATION_COUNT; i++) {
            keyhit_animation_t *kha = &keyhits[i];
            switch(kha->state) {
            case KA_RISE:
                obj->screen->draw_rect(obj->screen, kha->center_x, kha->current_y, 1, 1);
                break;
            case KA_BOOM:
                for(int j = 0; j < KEY_POINT_COUNT; j++) {
                    keyhit_point_t *pt = &kha->points[j];
                    if (keyhit_point_valid(pt, kha->center_x, kha->center_y)) {
                        obj->screen->draw_rect(obj->screen, (uint8_t)pt->x, (uint8_t)pt->y, 1, 1);
                    }
                }
                break;
            case KA_IDLE:
            default:
                break;
            }
        }

        if (pt_update) {
            keyhit_point_update(false);
            obj->ticks = timer_read32();
        }

        obj->screen->refresh(obj->screen);
        obj->dirty = false;
        //obj->ticks = timer_read32();
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
    if (IS_PRESSED(event)) {
        keyhit_point_update(true);
        kbddisp_debug("keyhit: col=%d, row=%d, pressed=%d, time=%d\n", event.key.col, event.key.row, event.pressed, event.time);
    }
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

#endif