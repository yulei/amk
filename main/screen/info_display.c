/**
 * @file info_display.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#if INFO_DISPLAY_NUM

#include <string.h>

#include "info_display.h"
#include "timer.h"
#include "wait.h"
#include "anim.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#include "deffont8.h"
#include "deffont16.h"

#ifndef INFO_DEBUG
#define INFO_DEBUG 1
#endif

#if INFO_DEBUG
#define info_debug  amk_printf
#else
#define info_debug(...)
#endif

#define GLYPH_MAX           128
#define GLYPH_BIG_WIDTH     16
#define GLYPH_BIG_HEIGHT    16
#define GLYPH_SMALL_WIDTH   8
#define GLYPH_SMALL_HEIGHT  8
#define GLYPH_VALID_MASK    0x8000
#define GLYPH_BUFFER_SIZE   ((GLYPH_MAX*GLYPH_BIG_WIDTH*GLYPH_BIG_HEIGHT)*2)

//uint8_t glyph_small_buf[8*8*2*GLYPH_MAX];
//uint8_t glyph_big_buf[16*16*2*GLYPH_MAX];

typedef struct {
    uint16_t codes[GLYPH_MAX];
    uint16_t width;
    uint16_t height;
    uint16_t format;
    uint8_t *glyphs;
    uint32_t glyph_count;
} font_t;

typedef struct {
    uint32_t    startup;    // startup time
    uint32_t    layer;      // layer state
    uint32_t    count;      // key count
    uint8_t     led;        // caps, num, scroll lock
} info_t;

typedef struct {
    screen_t        *screen;
    display_t       *display;
    display_param_t param;
    uint8_t         *buffer;
    uint32_t        buffer_size;
    uint8_t         used;
    bool            enabled;
    uint32_t        ticks;
    uint32_t        delay;
    info_t          info;
    font_t          big_font;
    font_t          small_font;
    uint8_t         glyph_small_buf[8*8*2*GLYPH_MAX];
    uint8_t         glyph_big_buf[16*16*2*GLYPH_MAX];
} info_display_obj_t;

static info_display_obj_t info_display_objs[INFO_DISPLAY_NUM];

static bool font_is_valid(font_t *font);
static void font_set_valid(font_t *font, bool valid);
static bool load_font_from_file(info_display_obj_t *obj, anim_t *anim, bool big);
static bool load_font_from_memory(info_display_obj_t *obj, bool big);
static void load_fonts(info_display_obj_t *obj);

static const uint8_t* get_glyph(font_t *font, uint16_t key);
static void draw_glyph(screen_t *screen, uint32_t x, uint32_t y, uint8_t width, uint8_t height, const uint8_t* glyph, uint16_t color);

static bool info_display_init(display_t *display, screen_t *screen)
{
    info_display_obj_t *obj = (info_display_obj_t*)display->data;
    obj->screen = screen;
    obj->buffer_size = obj->param.width*obj->param.height*2;
    obj->buffer = screen->get_buffer(screen);
    memset(obj->buffer, 0, obj->buffer_size);
    obj->ticks = timer_read32();
    obj->delay = 0;
    memset(&obj->info, 0, sizeof(info_t));
    memset(&obj->big_font, 0, sizeof(font_t));
    obj->big_font.glyphs = &obj->glyph_big_buf[0];
    memset(&obj->small_font, 0, sizeof(font_t));
    obj->small_font.glyphs = &obj->glyph_small_buf[0];
    load_fonts(obj);
    return true;
}

void info_display_uninit(display_t *display)
{
}

void info_display_task(display_t *display)
{
    info_display_obj_t *obj = (info_display_obj_t*)display->data;

    if (!obj->enabled) return;

    uint16_t color = 0xFFFF;

    const uint8_t *glyph = get_glyph(&obj->big_font, 'C');
    draw_glyph(obj->screen, 0, 0, 16, 16, glyph, color);

    glyph = get_glyph(&obj->big_font, 'N');
    draw_glyph(obj->screen, 16, 0, 16, 16, glyph, color);

    glyph = get_glyph(&obj->small_font, 'S');
    draw_glyph(obj->screen, 32, 0, 8, 8, glyph, color);
    glyph = get_glyph(&obj->small_font, 'C');
    draw_glyph(obj->screen, 40, 0, 8, 8, glyph, color);
    glyph = get_glyph(&obj->small_font, 'A');
    draw_glyph(obj->screen, 32, 8, 8, 8, glyph, color);
    glyph = get_glyph(&obj->small_font, 'W');
    draw_glyph(obj->screen, 40, 8, 8, 8, glyph, color);

    glyph = get_glyph(&obj->big_font, '0');
    draw_glyph(obj->screen, 48, 0, 16, 16, glyph, color);

    glyph = get_glyph(&obj->big_font, '0');
    draw_glyph(obj->screen, 64, 0, 16, 16, glyph, color);


    obj->screen->refresh(obj->screen);
}

void info_display_set_enable(display_t *display, bool enable)
{
    info_display_obj_t *obj = (info_display_obj_t*)display->data;
    obj->enabled = enable;
}

bool info_display_is_enabled(display_t *display)
{
    info_display_obj_t *obj = (info_display_obj_t*)display->data;

    return obj->enabled;
}

bool info_display_create(display_t *display, display_param_t *param)
{
    info_display_obj_t *obj = NULL;
    for (int i = 0; i < INFO_DISPLAY_NUM; i++) {
        if (!info_display_objs[i].used) {
            obj = &info_display_objs[i];
            break;
        }
    }

    if (obj) {
        obj->display = display;
        obj->param = *param;
        obj->used = 1;
        obj->enabled = param->enabled;

        display->data = obj;
        display->init = info_display_init;
        display->uninit = info_display_uninit;
        display->task = info_display_task;
        display->set_enable = info_display_set_enable;
        display->is_enabled = info_display_is_enabled;
        display->matrix_change = NULL;
        return true;
    }

    return false;
}


bool font_is_valid(font_t *font)
{
    return (font->format & GLYPH_VALID_MASK) ? true : false;
}

void font_set_valid(font_t *font, bool valid)
{
    if (valid) {
        font->format |= GLYPH_VALID_MASK;
    } else {
        font->format &= ~GLYPH_VALID_MASK;
    }
}

// font initialization
bool load_font_from_file(info_display_obj_t *obj, anim_t *anim, bool big)
{
    uint16_t width = big ? GLYPH_BIG_WIDTH : GLYPH_SMALL_WIDTH;
    uint16_t height = big ? GLYPH_BIG_HEIGHT: GLYPH_SMALL_HEIGHT;

    if ((anim_get_width(anim)!=width) 
        || (anim_get_height(anim)!=height)
        || (anim_get_frames(anim)>GLYPH_MAX)) {
        return false;
    }

    font_t *font = big ? &obj->big_font : &obj->small_font;
    font->width = anim_get_width(anim);
    font->height = anim_get_height(anim);
    font->glyph_count = anim_get_frames(anim);

    uint32_t glyph_size = (font->width*font->height)/8;

    for (int i = 0; i < font->glyph_count; i++) {
        uint16_t code = anim_get_delay(anim, i);
        font->codes[i] = code;
        anim_get_glyph_bitmap(anim, code, font->glyphs + i*glyph_size, glyph_size);
    }

    return true;
}

bool load_font_from_memory(info_display_obj_t *obj, bool big)
{
    font_t *font = big ? &obj->big_font : &obj->small_font;
    font->width = big ? GLYPH_BIG_WIDTH : GLYPH_SMALL_WIDTH;
    font->height = big ? GLYPH_BIG_HEIGHT: GLYPH_SMALL_HEIGHT;
    if (big) {
        font->glyph_count = strlen(deffont16_charset);
        for (int i = 0; i < font->glyph_count; i++) {
            font->codes[i] = deffont16_charset[i];
        }
        memcpy(font->glyphs, deffont16_table, sizeof(deffont16_table));
    } else {
        font->glyph_count = strlen(deffont8_charset);
        for (int i = 0; i < font->glyph_count; i++) {
            font->codes[i] = deffont8_charset[i];
        }
        memcpy(font->glyphs, deffont8_table, sizeof(deffont8_table));
    }
    return true;
}

static void load_fonts(info_display_obj_t *obj)
{
    anim_t *anim = anim_open(NULL, ANIM_TYPE_GLYPH);
    if (anim) {
        for (int i = 0; i < anim_total(anim); i++) {
            if( !font_is_valid(&obj->big_font)) {
                // big font not ready
                if (load_font_from_file(obj, anim, true)) {
                    font_set_valid(&obj->big_font, true);
                }
            }

            if( !font_is_valid(&obj->small_font)) {
                // small font not ready
                if (load_font_from_file(obj, anim, false)) {
                    font_set_valid(&obj->small_font, true);
                }
            }

            if( font_is_valid(&obj->big_font) && font_is_valid(&obj->small_font)) {
                anim_close(anim);
                // all done
                return;
            }
            anim_next(anim);
        }

        anim_close(anim);
    } 

    if( !font_is_valid(&obj->big_font)) {
        // big font not ready
        load_font_from_memory(obj, true);
    }

    if( !font_is_valid(&obj->small_font)) {
        // small font not ready
        load_font_from_memory(obj, false);
    }
}

static const uint8_t* get_glyph(font_t *font, uint16_t key)
{
    for (int i = 0; i < font->glyph_count; i++) {
        if (key == font->codes[i]) {
            return &font->glyphs[(i*font->width*font->height*2)];
        }
    }

    return NULL;
}

static void draw_glyph(screen_t *screen, uint32_t x, uint32_t y, uint8_t width, uint8_t height, const uint8_t* glyph, uint16_t color)
{
    const uint16_t *cur_g = (const uint16_t*)glyph;
    uint32_t cur_x = x;
    uint32_t cur_y = y;

    for (int i = 0; i < height; i++) {
        cur_x = x;
        for (int j = 0; j < width; j++) {
            screen->plot(screen, cur_x, cur_y, 0xFFFF - *cur_g);
            cur_x++;
            cur_g++;
        }
        cur_y++;
    }
}

#endif
