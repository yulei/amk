/*
 * @file display.c
 */

#include <string.h>

#include "timer.h"
#include "led.h"
#include "amk_gpio.h"
#include "usb_descriptors.h"
#include "usb_interface.h"
#include "wait.h"

#ifndef DISP_DEBUG
#define DISP_DEBUG 0
#endif

#if DISP_DEBUG
#define disp_debug  amk_printf
#else
#define disp_debug(...)
#endif

#include "screen.h"
#include "mscusb.h"
#include "anim.h"
#include "amk_printf.h"
#include "ff.h"
//#include "cmsis_os2.h"

void disp_init(void);

extern RTC_HandleTypeDef hrtc;
static void reset_to_msc(bool msc)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, msc ? 1 : 0);
    usb_connect(false);
    HAL_Delay(10);
    HAL_NVIC_SystemReset();
}

//osSemaphoreId_t dispSema;

static uint32_t last_ticks = 0;

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

#ifdef DATETIME_ENABLE
#define AMFT_WIDTH      10
#define AMFT_HEIGHT     30
#define AMFT_FRAMES     11
static uint16_t font_buf[AMFT_WIDTH*AMFT_HEIGHT*AMFT_FRAMES];
static bool first_screen = true;
#endif

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

#ifdef DATETIME_ENABLE
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
                    disp_debug("Datetime check: signature invalid\n");
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
            disp_debug("Font load: failed to load default font\n");
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

void render_datetime(render_t *render)
{
    if (!rtc_datetime_dirty) {
        return;
    }

    //osSemaphoreAcquire(dispSema, osWaitForever);

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

#endif

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

    //osSemaphoreAcquire(dispSema, osWaitForever);
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
                //anim_close(render->anim);
                //render->anim = NULL;
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
    disp_init();
}

static void render_screen(uint8_t index)
{
#ifdef DATETIME_ENABLE
    if ((index == 1) && (rtc_datetime_mode)){
        render_datetime(&renders[index]);
    } else
#endif
     {
        render_task(&renders[index]);
    }
}

void msc_task_kb(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    if (!screen_enable) return;

#ifdef DATETIME_ENABLE
    rtc_datetime_update();
#endif

    render_screen(0);
    render_screen(1);
}

void toggle_screen(void)
{
    screen_enable = !screen_enable;
    set_screen_state(screen_enable);
    disp_debug("screen enabled: %d\n", screen_enable);
}

void toggle_msc(void)
{
    reset_to_msc((usb_setting & USB_MSC_BIT) ? false : true);
}

void disp_thread(void *argument);

void disp_init(void) 
{
    set_screen_state(screen_enable);

    gpio_set_output_pushpull(FLASH_CS);
    gpio_write_pin(FLASH_CS, 1);

    last_ticks = timer_read32();
    // initialize renders
    for (int i = 0; i < sizeof(renders)/sizeof(render_t); i++) {
        renders[i].anim = anim_open(NULL, renders[i].type);
        if (renders[i].anim) {
            disp_debug("ANIM: faield to open root path\n");
        }
    }
#ifdef DATETIME_ENABLE
    font_init();
    rtc_datetime_init();
#endif
    if (!(usb_setting & USB_MSC_BIT)) {
        //osThreadNew(disp_thread, NULL, NULL);
    }
}

#if 0
void disp_thread(void *argument)
{
    dispSema = osSemaphoreNew(1, 1, NULL);

    for(;;) {
        if (!screen_enable) {
            osDelay(10);
            continue;
        };
#ifdef DATETIME_ENABLE
        rtc_datetime_update();
#endif

        render_screen(0);
        render_screen(1);
    }
}

extern SPI_HandleTypeDef hspi1;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1) {
        osSemaphoreRelease(dispSema);
    }
    disp_debug("spi tx cp callback\n");
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    disp_debug("spi txrx cp callback\n");
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1) {
        osSemaphoreRelease(dispSema);
    }

    disp_debug("spi failed to transfer: id=%d\n", (hspi==&hspi1) ? 1 : 2);
}
#endif