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
//    {ANIM_DISPLAY, 0, 72, 40, ANM_FILE, 0, 1},
};
const keyboard_key_t keyboard_keys[KEYBOARD_KEY_COUNT] = {
// x, y, w, h, pos.col, pos.row
    { 5,20, 3, 3, { 0,0}},
    { 9,20, 3, 3, { 1,0}},
    {13,20, 3, 3, { 2,0}},
    {17,20, 3, 3, { 3,0}},
    {21,20, 3, 3, { 4,0}},
    {25,20, 3, 3, { 5,0}},
    {29,20, 3, 3, { 6,0}},
    {33,20, 3, 3, { 7,0}},
    {37,20, 3, 3, { 8,0}},
    {41,20, 3, 3, { 9,0}},
    {45,20, 3, 3, {10,0}},
    {49,20, 3, 3, {11,0}},
    {53,20, 3, 3, {12,0}},
    {57,20, 6, 3, {14,0}},

    { 5,24, 4, 3, { 0,1}},
    {10,24, 3, 3, { 1,1}},
    {14,24, 3, 3, { 2,1}},
    {18,24, 3, 3, { 3,1}},
    {22,24, 3, 3, { 4,1}},
    {26,24, 3, 3, { 5,1}},
    {30,24, 3, 3, { 6,1}},
    {34,24, 3, 3, { 7,1}},
    {38,24, 3, 3, { 8,1}},
    {42,24, 3, 3, { 9,1}},
    {46,24, 3, 3, {10,1}},
    {50,24, 3, 3, {11,1}},
    {54,24, 3, 3, {12,1}},
    {58,24, 4, 3, {13,1}},
    {63,24, 3, 3, {14,1}},

    { 5,28, 6, 3, { 0,2}},
    {12,28, 3, 3, { 1,2}},
    {16,28, 3, 3, { 2,2}},
    {20,28, 3, 3, { 3,2}},
    {24,28, 3, 3, { 4,2}},
    {28,28, 3, 3, { 5,2}},
    {32,28, 3, 3, { 6,2}},
    {36,28, 3, 3, { 7,2}},
    {40,28, 3, 3, { 8,2}},
    {44,28, 3, 3, { 9,2}},
    {48,28, 3, 3, {10,2}},
    {52,28, 3, 3, {11,2}},
    {56,28, 6, 3, {13,2}},
    {63,28, 3, 3, {14,2}},

    { 5,32, 7, 3, { 0,3}},
    {13,32, 3, 3, { 2,3}},
    {17,32, 3, 3, { 3,3}},
    {21,32, 3, 3, { 4,3}},
    {25,32, 3, 3, { 5,3}},
    {29,32, 3, 3, { 6,3}},
    {33,32, 3, 3, { 7,3}},
    {37,32, 3, 3, { 8,3}},
    {40,32, 3, 3, { 9,3}},
    {45,32, 3, 3, {10,3}},
    {49,32, 3, 3, {11,3}},
    {53,32, 5, 3, {12,3}},
    {59,32, 3, 3, {13,3}},
    {63,32, 3, 3, {14,3}},

    { 5,36, 4, 3, { 0,4}},
    {10,36, 4, 3, { 1,4}},
    {15,36, 4, 3, { 2,4}},
    {20,36,20, 3, { 7,4}},
    {41,36, 4, 3, {10,4}},
    {46,36, 4, 3, {11,4}},
    {55,36, 3, 3, {12,4}},
    {59,36, 3, 3, {13,4}},
    {63,36, 3, 3, {14,4}},
};

#endif
