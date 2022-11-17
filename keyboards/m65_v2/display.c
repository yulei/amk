/*
 * @file display.c
 */

#include <string.h>
#include <stdbool.h>

#include "timer.h"
#include "led.h"
#include "amk_gpio.h"
#include "usb_descriptors.h"
#include "wait.h"

#ifndef DISP_DEBUG
#define DISP_DEBUG 1
#endif

#if DISP_DEBUG
#define disp_debug  amk_printf
#else
#define disp_debug(...)
#endif

#ifdef DYNAMIC_CONFIGURATION
extern RTC_HandleTypeDef hrtc;
static void reset_to_msc(bool msc);
#endif

#include "screen.h"
#include "mscusb.h"
#include "anim.h"
#include "amk_eeprom.h"
#include "amk_printf.h"
#include "ff.h"

#include "ssd1357.h"
static screen_driver_t ssd1357_driver;
static screen_driver_param_t ssd1357_driver_params = {
    INVALID_PIN,
    SCREEN_0_PWR,
    0,
    SCREEN_0_RESET,
    SCREEN_0_CS,
    SCREEN_0_DC,
    INVALID_PIN,
    INVALID_PIN,
    SPI_LCD_SSD1357,
};

static void screen_ssd1357_init(void)
{
    ssd1357_config(&ssd1357_driver, &ssd1357_driver_params);
    ssd1357_init(&ssd1357_driver);
}

///////////////////
enum {
    MODE_SINGLE,
    MODE_SEQUENCE,
    MODE_RANDOM,
    MODE_MAX
};

typedef struct {
    uint32_t type;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t delay;
    uint32_t mode;
    anim_t  *anim;
    uint16_t *buf;
    uint32_t buf_size;
    uint32_t ticks;
    bool filling;
} render_t;

#define BYTE_PER_PIXEL  2
#define ANIM_X_START    2
#define ANIM_Y_START    2
#define ANIM_WIDTH      60
#define ANIM_HEIGHT     60
static uint16_t anim_buf[ANIM_WIDTH*ANIM_HEIGHT];

static bool screen_enable = true;
static bool filling = false;
static render_t renders[] = {
    {
        .type = ANIM_TYPE_MAIN,
        .x = ANIM_X_START,
        .y = ANIM_Y_START,
        .width = ANIM_WIDTH,
        .height = ANIM_HEIGHT,
        .delay = 0,
        .mode = MODE_SEQUENCE,
        .anim = NULL,
        .buf = anim_buf,
        .buf_size = ANIM_WIDTH*ANIM_HEIGHT*BYTE_PER_PIXEL,
        .ticks = 0,
    },
};

#ifdef  TYPING_SPEED
#define TYPING_INTERVAL     100
#define TYPING_SPEED_MAX    10

bool typing_enable = false;
static uint32_t typing_speed    = 0;
static uint32_t typing_last     = 0;
static uint32_t typing_counter  = 0;
static uint32_t typing_update   = 0;

void hook_matrix_change_typing(keyevent_t event)
{
    typing_last = timer_read32();

    if (!typing_enable) return;

    typing_counter++;
}

void update_speed(void)
{
    uint32_t elapsed = timer_elapsed32(typing_update);
    if(elapsed > TYPING_INTERVAL) {
        if(typing_counter) {
            typing_speed = ((typing_speed + typing_counter) > TYPING_SPEED_MAX) ? TYPING_SPEED_MAX : (typing_speed + typing_counter);
            typing_counter = 0;
        } else {
            typing_speed = (typing_speed > 0) ? (typing_speed-1) : 0;
        }
        typing_update = timer_read32();
    }
}
#endif

static bool need_refresh(uint32_t last, uint32_t delay)
{
    uint32_t elapsed = timer_elapsed32(last);
    #ifdef TYPING_SPEED
    if (typing_enable) {
        update_speed();
        return (elapsed*typing_speed) > delay;
    } else {
        return elapsed > delay;
    }
    #else
        return elapsed > delay;
    #endif
}

static uint32_t last_ticks = 0;

static void set_screen_state(bool enable)
{
    if (enable) {
        gpio_set_output_pushpull(SCREEN_0_PWR);
        gpio_write_pin(SCREEN_0_PWR, SCREEN_0_PWR_EN);
        wait_ms(1);
        //screen_init();
        screen_ssd1357_init();
    } else {
        gpio_set_output_pushpull(SCREEN_0_PWR);
        gpio_write_pin(SCREEN_0_PWR, !SCREEN_0_PWR_EN);
        wait_ms(1);
    }
}

void matrix_init_kb(void)
{
    gpio_set_output_pushpull(FLASH_CS);
    gpio_write_pin(FLASH_CS, 1);

    gpio_set_output_pushpull(SCREEN_0_RESET);
    gpio_write_pin(SCREEN_0_RESET, 0);

    gpio_set_output_pushpull(SCREEN_0_CS);
    gpio_write_pin(SCREEN_0_CS, 0);

    gpio_set_output_pushpull(SCREEN_0_DC);
    gpio_write_pin(SCREEN_0_DC, 0);

    set_screen_state(screen_enable);
    last_ticks = timer_read32();
}

void render_task(render_t* render)
{
    if (!render->anim)
        return;

    if (filling) {
        //if (screen_fill_ready()) {
        if(ssd1357_fill_ready(&ssd1357_driver)){
            filling = false;
        } else {
            return;
        }
    };

    if ( need_refresh(render->ticks, render->delay)) {
        if ( 0 == anim_step(render->anim, &render->delay, render->buf, render->buf_size)) {
            bool play = false;
            switch(render->mode) {
                case MODE_SINGLE:
                    if (anim_rewind(render->anim)) {
                        play = true;
                    }
                    break;
                case MODE_SEQUENCE:
                    if (anim_next(render->anim)) {
                        play = true;
                    }
                    break;
                case MODE_RANDOM:
                    if (anim_random(render->anim)) {
                        play = true;
                    }
                    break;
                default:
                    break;
            }
            if (play) {
                anim_step(render->anim, &render->delay, render->buf, render->buf_size);
            } else {
                //anim_close(render->anim);
                //render->anim = NULL;
            }
        }
        //st7735_fill_rect_async(&st7735_driver, render->x, render->y, render->width, render->height, render->buf, render->buf_size);
        ssd1357_fill_rect(&ssd1357_driver, render->x, render->y, render->width, render->height, render->buf, render->buf_size);
        filling = true;
        render->ticks = timer_read32();
    }
}

void msc_init_kb(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    if (!anim_mount(true) ) reset_to_msc(true);

    memset(anim_buf, 0, sizeof(anim_buf));

    for (int i = 0; i < sizeof(renders)/sizeof(render_t); i++) {
        renders[i].anim = anim_open(NULL, renders[i].type);
        if (renders[i].anim) {
            disp_debug("ANIM: faield to open root path\n");
        }
    }
}

static void render_screen(uint8_t index)
{
    render_task(&renders[index]);
}

void msc_task_kb(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    if (!screen_enable) return;

    render_screen(0);
}

void screen_task_kb(void)
{
}

#include "action.h"
#include "action_layer.h"
#include "usb_interface.h"

bool hook_process_action_main(keyrecord_t *record)
{
    if (IS_NOEVENT(record->event) || !record->event.pressed) { 
        return false;
    }
    action_t action = layer_switch_get_action(record->event);
    if (action.kind.id != ACT_MODS) {
        return false;
    }

    switch(action.key.code) {
#ifdef TYPING_SPEED
        case KC_F14:
            typing_enable = !typing_enable;
            if(typing_enable) update_speed();
            disp_debug("typing enabled: %d\n", typing_enable);
            return true;
#endif
        case KC_F16:
            screen_enable = !screen_enable;
            set_screen_state(screen_enable);
            disp_debug("screen enabled: %d\n", screen_enable);
            return true;
        case KC_F21: {
            render_t *render = NULL;
            render = &renders[0];
            render->mode = (render->mode+1) % MODE_MAX;
        } return true;
        //case KC_F23:
        //    msc_erase();
        //    reset_to_msc(true);
        //    return true;
        case KC_F24: 
            reset_to_msc((usb_setting & USB_MSC_BIT) ? false : true);
            return true;
        default:
            break;
    }

    return false;
}

static void reset_to_msc(bool msc)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, msc ? 1 : 0);
    usb_connect(false);
    HAL_Delay(10);
    HAL_NVIC_SystemReset();
}
