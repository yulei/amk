/*
 * m65.c
 */

#include <string.h>

#include "m65.h"
#include "timer.h"
#include "led.h"
#include "gpio_pin.h"
#include "usb_descriptors.h"

#ifdef SCREEN_ENABLE
#include "screen.h"
#include "fractal.h"
#endif

#ifdef MSC_ENABLE
#include "anim.h"
#include "amk_printf.h"
enum {
    MODE_SINGLE,
    MODE_SEQUENCE,
    MODE_RANDOM
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

#define ANIM_X_START    0
#define ANIM_Y_START    0
#define ANIM_WIDTH      80
#define ANIM_HEIGHT     80
static uint16_t anim_buf[ANIM_WIDTH*ANIM_HEIGHT];

#define AUXI_X_START    0
#define AUXI_Y_START    92
#define AUXI_WIDTH      80
#define AUXI_HEIGHT     30
static uint16_t auxi_buf[AUXI_WIDTH*AUXI_HEIGHT];

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
        .buf_size = ANIM_WIDTH*ANIM_HEIGHT*2,
        .ticks = 0,
    },
    {
        .type = ANIM_TYPE_AUX,
        .x = AUXI_X_START,
        .y = AUXI_Y_START,
        .width = AUXI_WIDTH,
        .height = AUXI_HEIGHT,
        .delay = 0,
        .mode = MODE_SEQUENCE,
        .anim = NULL,
        .buf = auxi_buf,
        .buf_size = AUXI_WIDTH*AUXI_HEIGHT*2,
        .ticks = 0,
    },
};

#endif

static uint32_t last_ticks = 0;
static void reset_to_msc(bool msc);

void matrix_init_kb(void)
{
    gpio_set_output_pushpull(SCREEN_0_PWR);
    gpio_write_pin(SCREEN_0_PWR, SCREEN_0_PWR_EN);

    gpio_set_output_pushpull(FLASH_CS);
    gpio_write_pin(FLASH_CS, 1);

    fractal_init();
    last_ticks = timer_read32();
}

#ifdef MSC_ENABLE
void render_task(render_t* render)
{
    if (!render->anim) {
        render->anim = anim_open(NULL, render->type);
        if (!render->anim) {
            amk_printf("ANIM: faield to open root path\n");
            return;
        }
    } 

    if (filling) {
        if (screen_fill_ready()) {
            filling = false;
        } else {
            return;
        }
    };

    uint32_t elapsed = timer_elapsed32(render->ticks);
    if ( elapsed > render->delay) {
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
                anim_close(render->anim);
                render->anim = NULL;
            }
        }
        screen_fill_rect_async(render->x, render->y, render->width, render->height, render->buf, render->buf_size);
        filling = true;
        render->ticks = timer_read32();
    }
}

void msc_init_kb(void)
{
    if (!anim_mount(true) ) reset_to_msc(true);

    memset(anim_buf, 0, sizeof(anim_buf));
    memset(auxi_buf, 0, sizeof(auxi_buf));
}

void msc_task_kb(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    for (int i = 0; i < sizeof(renders)/sizeof(render_t); i++) {
        render_task(&renders[i]);
    }
}
#endif

#ifdef SCREEN_ENABLE
void screen_task_kb(void)
{
}
#endif

void led_set(uint8_t led)
{
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        amk_printf("turn caps on\n");
        gpio_write_pin(CAPS_LED_PIN, 1);
    } else {
        amk_printf("turn caps off\n");
        gpio_write_pin(CAPS_LED_PIN, 0);
    }
}

#ifdef DYNAMIC_CONFIGURATION
#include "action.h"
#include "action_layer.h"
#include "tusb.h"
extern RTC_HandleTypeDef hrtc;
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
        case KC_F24: 
            reset_to_msc((usb_setting & USB_MSC_BIT) ? false : true);
        return true;
        default:
            break;
    }

    return false;
}

void reset_to_msc(bool msc)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, msc ? 1 : 0);
    tud_disconnect();
    HAL_Delay(10);
    HAL_NVIC_SystemReset();
}

#endif