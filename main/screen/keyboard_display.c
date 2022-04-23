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

#define FWK_POINT_COUNT         10
#define FWK_ANIMATION_COUNT     40
#define FWK_FRAME_SIZE          20
#define FWK_SPEED_MAX           10
#define FWK_SPEED_MIN           1
#define FWK_ANGLE_STEP          16

#define ECG_FRAME_MAX           70
#define ECG_FRAME_MIN           2
#define ECG_POINT_COUNT         3
#define ECG_ANIMATION_COUNT     10
#define ECG_WIDTH_MIN           5
#define ECG_WIDTH_MAX           7
#define ECG_LINE_Y              12

enum {
    KEYHIT_FWK,
    KEYHIT_ECG,
};

enum {
    FWK_IDLE,
    FWK_RISE,
    FWK_BOOM,
};

enum {
    ECG_IDLE,
    ECG_MOVE,
};

typedef struct {
    int8_t x;
    int8_t y;
} ecg_point_t;

typedef struct {
    ecg_point_t points[ECG_POINT_COUNT];
    uint8_t current_x;
    uint8_t state;
} ecg_t;

typedef struct {
    ecg_t   ecgs[ECG_ANIMATION_COUNT];
    uint8_t start;
    uint8_t current;
} ecg_animation_t;

typedef struct {
    float   x;
    float   y;
    float   speed;
    float   angle;
} fwk_point_t;

typedef struct {
    fwk_point_t points[FWK_POINT_COUNT];
    uint8_t center_x;
    uint8_t center_y;
    uint8_t current_y;
    uint8_t state;
} fwk_t;

typedef struct {
    fwk_t fwks[FWK_ANIMATION_COUNT];
    uint8_t current;
} fwk_animation_t;

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

static fwk_animation_t fwk_anim;
static ecg_animation_t ecg_anim;

static uint8_t keyhit_mode = KEYHIT_FWK;

static void draw_rect(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
static void draw_line(screen_t *screen, int32_t x0, int32_t y0, int32_t x1, int32_t y1);

static bool fwk_point_valid(fwk_point_t *point, uint8_t center_x, uint8_t center_y)
{
    if (!(point->x < FRAME_WIDTH) || !(point->x > 0.0)) return false;
    if (!(point->y < FRAME_HEIGHT) || !(point->y > 0.0)) return false;

    if (point->x > (center_x+FWK_FRAME_SIZE)) return false;
    if (point->x < (center_x-FWK_FRAME_SIZE)) return false;

    if (point->y > (center_y+FWK_FRAME_SIZE)) return false;
    if (point->y < (center_y-FWK_FRAME_SIZE)) return false;

    return true;
}

static bool ecg_wave_valid(ecg_t *ecg)
{
    uint32_t min_x = 0xFF;
    for (int i = 0; i < ECG_POINT_COUNT; i++) {
        if((ecg->points[i].x+ecg->current_x) < min_x) {
            min_x = ecg->points[i].x+ecg->current_x;
        }
    }

    if (min_x > ECG_FRAME_MAX) {
        return false;
    } else {
        return true;
    }
}

static void keyhit_fwk_init(void)
{
    fwk_anim.current = 0;

    for (int i = 0; i < FWK_ANIMATION_COUNT; i++) {
        fwk_t *fwk = &fwk_anim.fwks[i];
        fwk->center_x = 0;
        fwk->center_y = 0;
        fwk->current_y = 0;
        fwk->state = FWK_IDLE;
        for (int j = 0; j < FWK_POINT_COUNT; j++) {
            fwk->points[j].x = 0.0;
            fwk->points[j].y = 0.0;
            fwk->points[j].speed = 0.0;
            fwk->points[j].angle = 0.0;
        }
    }
}

static void keyhit_fwk_update(bool hit)
{
    if (hit) {
        fwk_t *fwk = &fwk_anim.fwks[fwk_anim.current];
        fwk->state = FWK_RISE;
        fwk->center_x = rand() % FRAME_WIDTH;
        fwk->center_y = rand() % FRAME_HEIGHT;
        fwk->current_y = 0;

        for (int i = 0; i< FWK_POINT_COUNT; i++) {
            fwk_point_t *pt = &fwk->points[i];
            pt->x = fwk->center_x;
            pt->y = fwk->center_y;
            pt->speed = rand() % FWK_SPEED_MAX;
            if (pt->speed < 1.0f) pt->speed = 1.0f;
            pt->angle = i*FWK_ANGLE_STEP;
        }
        fwk_anim.current = (fwk_anim.current+1) % FWK_ANIMATION_COUNT;
    } else {
        for (int i = 0; i < FWK_ANIMATION_COUNT; i++) {
            fwk_t *fwk= &fwk_anim.fwks[i];
            switch(fwk->state) {
            case FWK_RISE:
                if (fwk->current_y < fwk->center_y) {
                    fwk->current_y++;
                } else {
                    fwk->state = FWK_BOOM;
                }
                break;
            case FWK_BOOM: {
                bool done = true;
                for(int j = 0; j < FWK_POINT_COUNT; j++) {
                    fwk_point_t *pt = &fwk->points[j];
                    if (fwk_point_valid(pt, fwk->center_x, fwk->center_y)) {
                        pt->x += pt->speed * sin((pt->angle/(FWK_ANGLE_STEP*(FWK_POINT_COUNT+1)*1.0)) * 2.0 * M_PI);
                        pt->y += pt->speed * cos((pt->angle/(FWK_ANGLE_STEP*(FWK_POINT_COUNT+1)*1.0)) * 2.0 * M_PI);
                        done = false;
                    }
                }
                if (done) {
                    fwk->state = FWK_IDLE;
                }
                }break;
            case FWK_IDLE:
            default:
                break;
            }
        }
    }
}

static void keyhit_fwk_render(screen_t *screen)
{
    //draw_line(screen, 1, 5, 60, 5);
    for (int i = 0; i < FWK_ANIMATION_COUNT; i++) {
        fwk_t *fwk = &fwk_anim.fwks[i];
        switch(fwk->state) {
        case FWK_RISE:
            draw_rect(screen, fwk->center_x, fwk->current_y, 1, 1);
            break;
        case FWK_BOOM:
            for(int j = 0; j < FWK_POINT_COUNT; j++) {
                fwk_point_t *pt = &fwk->points[j];
                if (fwk_point_valid(pt, fwk->center_x, fwk->center_y)) {
                    draw_rect(screen, (uint8_t)pt->x, (uint8_t)pt->y, 1, 1);
                }
            }
            break;
        case FWK_IDLE:
        default:
            break;
        }
    }
}

static void keyhit_ecg_init(void)
{
    ecg_anim.start      = 0;
    ecg_anim.current    = 0;
    for (int i = 0; i < ECG_ANIMATION_COUNT; i++) {
        ecg_t *ecg = &ecg_anim.ecgs[i];
        ecg->current_x = 0;
        for (int j = 0; j < ECG_POINT_COUNT; j++) {
            ecg->points[j].x = 0;
            ecg->points[j].y = 0;
        }
    }
}

static void keyhit_ecg_update(bool hit)
{
    if (hit) {
        ecg_t *ecg = &ecg_anim.ecgs[ecg_anim.current];
        ecg->state = ECG_MOVE;
        ecg->current_x = 0;
        ecg->points[0].x = 0;
        ecg->points[0].y = ECG_LINE_Y;
        ecg->points[1].x = 2;
        ecg->points[1].y = ECG_LINE_Y-10;
        ecg->points[2].x = 4;
        ecg->points[2].y = ECG_LINE_Y;
        //for (int i = 0; i < ECG_POINT_COUNT; i++) {
        //    ecg->points[i].x = i*2;
        //    ecg->points[i].y = ECG_LINE_Y - ;
        //}

        ecg_anim.current = (ecg_anim.current+1) % ECG_ANIMATION_COUNT;
    } else {
        for (int i = 0; i< ECG_ANIMATION_COUNT; i++) {
            ecg_t *ecg = &ecg_anim.ecgs[i];
            if (ecg->state == ECG_MOVE) {
                ecg->current_x++;
                if (!ecg_wave_valid(ecg)) {
                    ecg->state = ECG_IDLE;
                }
            }
        }
    }
}

static void keyhit_ecg_render(screen_t *screen)
{
    for (int i = 0; i < ECG_ANIMATION_COUNT; i++) {
        ecg_t *ecg = &(ecg_anim.ecgs[i]);
        if (ecg->state == ECG_IDLE) continue;

        for (int j = 0; j < ECG_POINT_COUNT-1; j++) {
            int32_t x0 = ecg->points[j].x+ecg->current_x;
            int32_t y0 = ecg->points[j].y;
            int32_t x1 = ecg->points[j+1].x+ecg->current_x;
            int32_t y1 = ecg->points[j+1].y;

            draw_line(screen, x0, y0, x1, y1);
        }
    }

    //uint32_t x = ecg_anim.ecgs[ecg_anim.start].current_x;
}

static void keyhit_init(void)
{
    srand(timer_read32());
    keyhit_fwk_init();
    keyhit_ecg_init();
}

static void keyhit_update(bool hit)
{
    if (keyhit_mode == KEYHIT_FWK) {
        keyhit_fwk_update(hit);
    } else {
        keyhit_ecg_update(hit);
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

static void draw_rect(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < width; i++) {
        for (uint32_t j = 0; j < height; j++) {
            screen->plot(screen, x+i, y+j, 0xFFFF);
        }
    }
}

static bool dot_valid(int32_t x0, int32_t y0)
{
    if ((x0 < ECG_FRAME_MIN) || (x0 > ECG_FRAME_MAX)) return false;

    return true;
}

static void draw_line(screen_t *screen, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    if ((x0==x1) && (y0==y1)) {
        if (dot_valid(x0, y0)) {
            screen->plot(screen, x0, y0, 0xFFFF);
        }
        return;
    }

    float delta_x = x1-x0;
    float delta_y = y1-y0;
    float step_x, step_y;
    uint32_t total;
    if (fabs(delta_x) > fabs(delta_y)) {
        step_x = delta_x > 0 ? 1.0 : -1.0;
        step_y = delta_y/fabs(delta_x);
        total = fabs(delta_x);
    } else {
        step_x = delta_x/fabs(delta_y);
        step_y = delta_y > 0 ? 1.0 : -1.0;
        total = fabs(delta_y);
    }

    float x = x0;
    float y = y0;
    uint32_t i = 0;
    do {
        if (dot_valid((int32_t)(x+0.5), (int32_t)(y+0.5))) {
            screen->plot(screen, (int32_t)(x+0.5), (int32_t)(y+0.5), 0xFFFF);
        }
        ++i;
        x += step_x;
        y += step_y;
    } while (i < total);
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
                draw_rect(obj->screen, keyboard_keys[i].x, keyboard_keys[i].y, keyboard_keys[i].w, keyboard_keys[i].h);
            }
        }

        if (keyhit_mode == KEYHIT_FWK) {
            keyhit_fwk_render(obj->screen);
        } else {
            keyhit_ecg_render(obj->screen);
        }

        if (pt_update) {
            keyhit_update(false);
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
        keyhit_update(true);
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