/*
 * m65.c
 */

#include <string.h>

#include "m65.h"
#include "timer.h"
#include "led.h"
#include "amk_gpio.h"
#include "usb_descriptors.h"
#include "wait.h"

#ifndef M65_DEBUG
#define M65_DEBUG 0
#endif

#if M65_DEBUG
#define m65_debug  amk_printf
#else
#define m65_debug(...)
#endif

#ifdef DYNAMIC_CONFIGURATION
extern RTC_HandleTypeDef hrtc;
static void reset_to_msc(bool msc);
#endif

#ifdef SCREEN_ENABLE
#include "screen.h"
#endif


#ifdef MSC_ENABLE
#include "mscusb.h"
#include "anim.h"
#include "amk_printf.h"
#include "rtc8563.h"
#include "def_font.h"
#include "ff.h"

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

#define AMFT_WIDTH      10
#define AMFT_HEIGHT     30
#define AMFT_FRAMES     11
static uint16_t font_buf[AMFT_WIDTH*AMFT_HEIGHT*AMFT_FRAMES];

static bool screen_enable = true;
static bool first_screen = true;
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
        .buf_size = AUXI_WIDTH*AUXI_HEIGHT*BYTE_PER_PIXEL,
        .ticks = 0,
    },
};

static rtc_datetime_t rtc_dt = {
    .second = 0,
    .minute = 42,
    .hour = 21,
    .day = 29,
    .month = 6,
    .year = 21,
};

static bool rtc_datetime_mode = false;
static bool rtc_datetime_dirty = false;
static uint32_t rtc_datetime_ticks = 0;
#define RTC_FILE_SIG            "AMDT"
#define RTC_FILE_SIZE           10
#define RTC_CHECKING_INTERVAL   700

static void rtc_datetime_scan(void);

static void rtc_datetime_init(void)
{
    rtc8563_init();

    rtc_datetime_scan();
    if (rtc_datetime_dirty) {
        rtc8563_write_time(&rtc_dt);
    }
}

static void rtc_datetime_scan(void)
{
    DIR dir;
    FRESULT res = f_opendir(&dir, "/");
    if (res == FR_OK) {
        for (;;) {
            FILINFO fno;
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0){
                break;
            }

            if ( (fno.fattrib & AM_DIR) == 0) {
                FIL file;
                if (FR_OK != f_open(&file, fno.fname, FA_READ)) {
                    //amk_printf("Datetime check: failed to open file: %s\n", fno.fname);
                    continue;
                }
                if (f_size(&file) != RTC_FILE_SIZE) {
                    f_close(&file);
                    //amk_printf("Datetime check: file size mismatch, skip file: %s\n", fno.fname);
                    continue;
                }
                uint8_t buf[RTC_FILE_SIZE];
                UINT readed = 0;
                if (FR_OK != f_read(&file, buf, sizeof(buf), &readed)) {
                    f_close(&file);
                    //amk_printf("Datetime check: failed to read file header: %s\n", fno.fname);
                    continue;
                }
                if (memcmp(RTC_FILE_SIG, buf, 4) != 0) {
                    f_close(&file);
                    m65_debug("Datetime check: signature invalid\n");
                    continue;
                }
                uint8_t *p      = &buf[4];
                rtc_dt.year     = *p++;
                rtc_dt.month    = *p++;
                rtc_dt.day      = *p++;
                rtc_dt.hour     = *p++;
                rtc_dt.minute   = *p++;
                rtc_dt.second   = *p++;
                rtc_datetime_dirty = true;
                f_close(&file);
                f_unlink(fno.fname);
                break;
            }
        }
        f_closedir(&dir);
    }
}

static void rtc_datetime_update(void)
{
    if (timer_elapsed32(rtc_datetime_ticks) > RTC_CHECKING_INTERVAL) {

        rtc_datetime_t dt = {0,0,0,0,0,0};
        rtc8563_read_time(&dt);
        if (rtc_dt.second != dt.second
            || rtc_dt.minute != dt.minute
            || rtc_dt.hour != dt.hour
            || rtc_dt.day != dt.day
            || rtc_dt.month != dt.month
            || rtc_dt.year != dt.year) {
            memcpy(&rtc_dt, &dt, sizeof(rtc_dt));
            rtc_datetime_dirty = true;
            //amk_printf("rtc datetime: %02d-%02d-%02d\n", rtc_dt.hour, rtc_dt.minute, rtc_dt.second);
        }
        rtc_datetime_ticks = timer_read32();
    }
}

static void rtc_datetime_inc_second(void)
{
    rtc_datetime_t dt = {0,0,0,0,0,0};
    rtc8563_read_time(&dt);
    dt.second += 1;
    dt.minute += dt.second/60;
    dt.hour += dt.minute/60;
    dt.minute %= 60;
    dt.second %= 60;
    rtc8563_write_time(&dt);
}

static void rtc_datetime_dec_second(void)
{
    rtc_datetime_t dt = {0,0,0,0,0,0};
    rtc8563_read_time(&dt);
    if (dt.second > 0) {
        dt.second--;
    } else {
        if (dt.minute > 0) {
            dt.minute--;
        } else {
            dt.hour--;
            dt.minute = 59;
        }
        dt.second = 59;
    }
    rtc8563_write_time(&dt);
}

static void font_init(void)
{
    memset(font_buf, 0, AMFT_WIDTH*AMFT_HEIGHT*BYTE_PER_PIXEL*AMFT_FRAMES);

    anim_t * anim = anim_open(NULL, ANIM_TYPE_FONT);
    if (anim) {
        if (anim_get_frames(anim) < AMFT_FRAMES) {
            anim_close(anim);
            anim = NULL;
        }
    } 

    if (anim == NULL) {
        if (!anim_load_font(default_font, font_buf, AMFT_FRAMES)) {
            m65_debug("Font load: failed to load default font\n");
        }
        return;
    }

    uint16_t *p = font_buf;
    for(uint32_t i = 0; i < AMFT_FRAMES; i++) {
        uint32_t delay = 0;
        anim_step(anim, &delay, p, AMFT_WIDTH*AMFT_HEIGHT*BYTE_PER_PIXEL);
        p += AMFT_WIDTH*AMFT_HEIGHT;
    }

    anim_close(anim);
}

#endif

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 0, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 4},
};
#endif

static uint32_t last_ticks = 0;

#ifdef SCREEN_ENABLE
static void set_screen_state(bool enable)
{
    if (enable) {
        gpio_set_output_pushpull(SCREEN_0_PWR);
        gpio_write_pin(SCREEN_0_PWR, SCREEN_0_PWR_EN);
        wait_ms(1);
        screen_init();
    } else {
        gpio_set_output_pushpull(SCREEN_0_PWR);
        gpio_write_pin(SCREEN_0_PWR, !SCREEN_0_PWR_EN);
        wait_ms(1);
    }
}
#endif

//#if defined(DYNAMIC_CONFIGURATION) || defined(MSC_ENABLE)
void matrix_init_kb(void)
{
#ifdef SCREEN_ENABLE
    set_screen_state(screen_enable);
#endif

    gpio_set_output_pushpull(FLASH_CS);
    gpio_write_pin(FLASH_CS, 1);

    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, 1);

    last_ticks = timer_read32();
    // initialize renders
#ifdef MSC_ENABLE
    for (int i = 0; i < sizeof(renders)/sizeof(render_t); i++) {
        renders[i].anim = anim_open(NULL, renders[i].type);
        if (renders[i].anim) {
            m65_debug("ANIM: faield to open root path\n");
        }
    }
    font_init();
    rtc_datetime_init();
#endif
}

#ifdef MSC_ENABLE
void render_datetime(render_t *render)
{
    if (!rtc_datetime_dirty) {
        return;
    }

    if (filling) {
        if (screen_fill_ready()) {
            filling = false;
        } else {
            return;
        }
    };

    char buffer[9];
    sprintf(buffer, "%02d:%02d:%02d", rtc_dt.hour, rtc_dt.minute, rtc_dt.second);
    //amk_printf("%s\n", buffer);
    memset(render->buf, 0, AUXI_WIDTH*AUXI_HEIGHT*BYTE_PER_PIXEL);
    for(int i = 0; i < 8; i++){
        uint32_t start_x = i*AMFT_WIDTH;
        uint32_t font_index = buffer[i]-'0';
        for (int y = 0; y < AMFT_HEIGHT; y++) {
            memcpy(&(render->buf[y*AUXI_WIDTH+start_x]), &font_buf[font_index*AMFT_WIDTH*AMFT_HEIGHT + y*AMFT_WIDTH], AMFT_WIDTH*BYTE_PER_PIXEL);
        }
    }

    screen_fill_rect_async(render->x, render->y, render->width, render->height, render->buf, render->buf_size);
    filling = true;
    rtc_datetime_dirty = false;
}

void render_task(render_t* render)
{
    if (!render->anim)
        return;

    /*if (!render->anim) {
        render->anim = anim_open(NULL, render->type);
        if (!render->anim) {
            amk_printf("ANIM: faield to open root path\n");
            return;
        }
    }*/

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
    if (usb_setting & USB_MSC_BIT) return;

    if (!anim_mount(true) ) reset_to_msc(true);

    memset(anim_buf, 0, sizeof(anim_buf));
    memset(auxi_buf, 0, sizeof(auxi_buf));
}

static void render_screen(uint8_t index)
{
    if ((index == 1) && (rtc_datetime_mode)){
        render_datetime(&renders[index]);
    } else {
        render_task(&renders[index]);
    }
}

void msc_task_kb(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    rtc_datetime_update();

    if (!screen_enable) return;

    render_screen(0);
    render_screen(1);
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
        m65_debug("turn caps on\n");
        gpio_write_pin(CAPS_LED_PIN, 0);
    } else {
        m65_debug("turn caps off\n");
        gpio_write_pin(CAPS_LED_PIN, 1);
    }
}

#ifdef DYNAMIC_CONFIGURATION
#include "action.h"
#include "action_layer.h"
#include "tusb.h"
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
        case KC_F16:
            screen_enable = !screen_enable;
            set_screen_state(screen_enable);
            m65_debug("screen enabled: %d\n", screen_enable);
            return true;
        case KC_F17:
            rtc_datetime_inc_second();
            m65_debug("datetime_mode: increase second\n");
            return true;
        case KC_F18:
            rtc_datetime_dec_second();
            m65_debug("datetime_mode: decrease second\n");
            return true;
        case KC_F19:
            rtc_datetime_mode = !rtc_datetime_mode;
            m65_debug("datetime_mode switch: %d\n", rtc_datetime_mode);
            return true;
        case KC_F20:
            first_screen = !first_screen;
            return true;
        case KC_F21: {
            render_t *render = NULL;
            if (first_screen) {
                render = &renders[0];
            } else {
                render = &renders[1];
            }
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


#endif

#ifdef DYNAMIC_CONFIGURATION
static void reset_to_msc(bool msc)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, msc ? 1 : 0);
    tud_disconnect();
    HAL_Delay(10);
    HAL_NVIC_SystemReset();
}
#endif