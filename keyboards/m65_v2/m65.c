/*
 * m65.c
 */

#include "m65.h"

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
    {0, 4, 4, 4},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 1, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 1, 4},
};
#endif

#ifdef RGB_INDICATOR_ENABLE
#include "led.h"
#include "rgb_indicator.h"
uint8_t g_rgb_indicator_index[RGB_INDICATOR_LED_NUM] = {0};
#define CAPS_LED    0

void led_set(uint8_t led)
{
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_indicator_set(CAPS_LED, 0xFF, 0xFF, 0xFF);
        amk_printf("turn caps on\n");
    } else {
        rgb_indicator_set(CAPS_LED, 0, 0, 0);
        amk_printf("turn caps off\n");
    }
}
#endif

#ifdef MSC_ENABLE
#include "usb_common.h"
#include "usb_interface.h"
#include "mscusb.h"
#include "anim.h"

#ifdef DYNAMIC_CONFIGURATION
extern RTC_HandleTypeDef hrtc;
static void reset_to_msc(bool msc)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, msc ? 1 : 0);
    usb_connect(false);
    HAL_Delay(10);
    HAL_NVIC_SystemReset();
}
#endif

void msc_init_kb(void)
{
    if (usb_setting & USB_MSC_BIT) return;
    
#ifdef DYNAMIC_CONFIGURATION
    if (!anim_mount(true) ) reset_to_msc(true);
#endif
}
#endif // MSC_ENABLE

#ifdef SCREEN_ENABLE
#include "render_common.h"
#include "keyboard_display.h"
screen_driver_param_t screen_driver_params[SCREEN_DRIVER_NUM] = {
    {INVALID_PIN, SCREEN_0_PWR, 0, SCREEN_0_RESET, SCREEN_0_CS, SCREEN_0_DC, INVALID_PIN, INVALID_PIN, SPI_LCD_SSD1306},
};

screen_param_t screen_params[SCREEN_NUM] = {
    {0, 0, 72, 40, 0},
};

display_param_t display_params[DISPLAY_NUM] = {
    // type, screen, width, height, file_type, audio_format, enable
    {KEYBOARD_DISPLAY, 0, 72, 40, 0, 0, 1},
//    {ANIM_DISPLAY, 0, 60, 60, ANM_FILE, 0, 1},
//    {AUDIO_DISPLAY, 0, 60, 60, 0, 0, 1},
};
const keyboard_key_t keyboard_keys[KEYBOARD_KEY_COUNT] = {
// x, y, w, h, pos.col, pos.row
    { 0, 0, 3, 3, { 0,0}},
    { 4, 0, 3, 3, { 1,0}},
    { 8, 0, 3, 3, { 2,0}},
    {12, 0, 3, 3, { 3,0}},
    {16, 0, 3, 3, { 4,0}},
    {20, 0, 3, 3, { 5,0}},
    {24, 0, 3, 3, { 6,0}},
    {28, 0, 3, 3, { 7,0}},
    {32, 0, 3, 3, { 8,0}},
    {36, 0, 3, 3, { 9,0}},
    {40, 0, 3, 3, {10,0}},
    {44, 0, 3, 3, {11,0}},
    {48, 0, 3, 3, {12,0}},
    {52, 0, 6, 3, {14,0}},

    { 0, 4, 4, 3, { 0,1}},
    { 5, 4, 3, 3, { 1,1}},
    { 9, 4, 3, 3, { 2,1}},
    {13, 4, 3, 3, { 3,1}},
    {17, 4, 3, 3, { 4,1}},
    {21, 4, 3, 3, { 5,1}},
    {25, 4, 3, 3, { 6,1}},
    {29, 4, 3, 3, { 7,1}},
    {33, 4, 3, 3, { 8,1}},
    {37, 4, 3, 3, { 9,1}},
    {41, 4, 3, 3, {10,1}},
    {45, 4, 3, 3, {11,1}},
    {49, 4, 3, 3, {12,1}},
    {53, 4, 4, 3, {13,1}},
    {58, 4, 3, 3, {14,1}},

    { 0, 8, 6, 3, { 0,2}},
    { 7, 8, 3, 3, { 1,2}},
    {11, 8, 3, 3, { 2,2}},
    {15, 8, 3, 3, { 3,2}},
    {19, 8, 3, 3, { 4,2}},
    {23, 8, 3, 3, { 5,2}},
    {27, 8, 3, 3, { 6,2}},
    {31, 8, 3, 3, { 7,2}},
    {35, 8, 3, 3, { 8,2}},
    {39, 8, 3, 3, { 9,2}},
    {43, 8, 3, 3, {10,2}},
    {47, 8, 3, 3, {11,2}},
    {51, 8, 6, 3, {13,2}},
    {58, 8, 3, 3, {14,2}},

    { 0,12, 7, 3, { 0,3}},
    { 8,12, 3, 3, { 2,3}},
    {12,12, 3, 3, { 3,3}},
    {16,12, 3, 3, { 4,3}},
    {20,12, 3, 3, { 5,3}},
    {24,12, 3, 3, { 6,3}},
    {28,12, 3, 3, { 7,3}},
    {32,12, 3, 3, { 8,3}},
    {36,12, 3, 3, { 9,3}},
    {40,12, 3, 3, {10,3}},
    {44,12, 3, 3, {11,3}},
    {48,12, 5, 3, {12,3}},
    {54,12, 3, 3, {13,3}},
    {58,12, 3, 3, {14,3}},

    { 0,16, 4, 3, { 0,4}},
    { 5,16, 4, 3, { 1,4}},
    {10,16, 4, 3, { 2,4}},
    {15,16,20, 3, { 7,4}},
    {36,16, 4, 3, {10,4}},
    {41,16, 4, 3, {11,4}},
    {50,16, 3, 3, {12,4}},
    {54,16, 3, 3, {13,4}},
    {58,16, 3, 3, {14,4}},
};

#endif
