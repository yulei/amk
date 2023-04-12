/**
 * @file uart_cmd.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "generic_hal.h"
#include "amk_cmd.h"
#include "amk_gpio.h"
#include "amk_printf.h"
#include "linear_buffer.h"
#include "usb_common.h"
#include "report.h"

#ifndef CMD_DEBUG
#define CMD_DEBUG 1
#endif

#if CMD_DEBUG
#define cmd_debug  amk_printf
#else
#define cmd_debug(...)
#endif

enum screen_adjust{
    SA_LEFT = 1,
    SA_RIGHT,
    SA_UP,
    SA_DOWN,
    SA_SAVE,
    SA_INC_HUE,
    SA_DEC_HUE,
    SA_INC_SAT,
    SA_DEC_SAT,
    SA_INC_VAL,
    SA_DEC_VAL,
    SA_CLR_RST,
};

extern UART_HandleTypeDef huart1;

static linear_buf_t cmd_buf;
static bool msc_on = false;
static bool screen_on = true;
static bool screen_adjust = false;

void uart_recv_char(uint8_t c)
{
    if (!linear_buf_size(&cmd_buf)) {
        linear_buf_put(&cmd_buf, c);
    } else {
        uint8_t data = 0;
        linear_buf_get(&cmd_buf, linear_buf_size(&cmd_buf)-1, &data);
        if (data != CMD_ENDL) {
            linear_buf_put(&cmd_buf, c);
        }
    }
}

static void process_cmd(cmd_t *cmd)
{
    cmd_debug("CMD: type=%d, status=%d\n", cmd->type, cmd->param.status);
}

void uart_cmd_init(void)
{
    linear_buf_init(&cmd_buf);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void uart_cmd_task(void)
{
    if (!linear_buf_size(&cmd_buf)) {
        return;
    }

    uint8_t data = 0;
    linear_buf_get(&cmd_buf, linear_buf_size(&cmd_buf)-1, &data);
    if (data != CMD_ENDL) {
        return;
    }
    cmd_t cmd;
    cmd_parse(linear_buf_data(&cmd_buf), linear_buf_size(&cmd_buf), &cmd);
    process_cmd(&cmd);

    linear_buf_init(&cmd_buf);
}

static uint8_t cmd_str[64];

void uart_cmd_send(cmd_t* cmd)
{
    int32_t size = cmd_compose(cmd, cmd_str, 64);
    if ((size > 0) && (size < 64)) {
        HAL_UART_Transmit(&huart1, cmd_str, size, HAL_MAX_DELAY);
        cmd_debug("CMD: %s\n", cmd_str);
    }
}

void matrix_init_kb(void)
{
    gpio_set_output_pushpull(USB_EN_PIN);
    gpio_write_pin(USB_EN_PIN, 1);

    uart_cmd_init();
}

void matrix_scan_kb(void)
{
    uart_cmd_task();
}

#include "quantum.h"
#include "usb_interface.h"

bool process_record_kb(uint16_t keycode, keyrecord_t *record)
{
    if (!record->event.pressed) {
        return true;
    }

    switch(keycode) {
    case KC_LEFT:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_LEFT;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_RIGHT:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_RIGHT;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_UP:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_UP;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_DOWN:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_DOWN;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_PGUP:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_INC_HUE;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_PGDN:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_DEC_HUE;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_HOME:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_INC_SAT;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_END:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_DEC_SAT;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_BSPC:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_INC_VAL;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_BSLS:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_DEC_VAL;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_ENT:
        if (!screen_adjust) return true;
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_CLR_RST;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_F15:
        screen_adjust = !screen_adjust;
        cmd_debug("screen adjust enabled: %d\n", screen_adjust);
        if (!screen_adjust) {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = SA_SAVE; // save to eeprom
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_F16:
        {
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_MODE;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_F17:
        {
            screen_on = !screen_on;
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_ADJUST;
            cmd.param.screen.state = 0;
            uart_cmd_send(&cmd);
        }
        return false;
    case KC_F24:
        {
            msc_on = !msc_on;
            cmd_t cmd = {0};
            cmd.type = CMD_SCREEN;
            cmd.param.screen.action = CMD_SCREEN_MSC;
            cmd.param.screen.state = msc_on ? 1 : 0;
            uart_cmd_send(&cmd);
            gpio_write_pin(USB_EN_PIN, msc_on ? 0 : 1);
            if (msc_on) {
                usb_setting |= USB_SWITCH_BIT;
            } else {
                usb_setting &= ~(USB_SWITCH_BIT);
            }
            cmd_debug("F24 pressed, msc=%d, usb_seeting=%x\n", msc_on, usb_setting);
        }
        return false;
    default:
        break;
    }

    return true;
}

extern void indicator_led_set(uint8_t led);

void led_set_kb(uint8_t led)
{
    cmd_t cmd = {0};
    cmd.type = CMD_LED;
    cmd.param.led = led;
    uart_cmd_send(&cmd);
    indicator_led_set(led);
}

#if 0
void hook_matrix_change_kb(keyevent_t event)
{
    cmd_t cmd = {0};
    cmd.type = CMD_KEYHIT;
    cmd.param.keyhit.row = event.key.row;
    cmd.param.keyhit.col = event.key.col;
    cmd.param.keyhit.pressed = event.pressed;
    uart_cmd_send(&cmd);
}
#endif

layer_state_t layer_state_set_kb(layer_state_t state)
{
    cmd_t cmd = {0};
    cmd.type = CMD_LAYER;
    cmd.param.layer = 0;

    for (int i = 31; i >= 0; i--) {
        if (layer_state & (1UL << i)) {
            cmd.param.layer = i;
            break;
        }
    }
    uart_cmd_send(&cmd);
    return layer_state_set_user(state);
}

void hook_send_report(uint32_t type, const void* data)
{
    if (type != HID_REPORT_ID_KEYBOARD) return;

    const report_keyboard_t *report = (const report_keyboard_t*)data;
    cmd_t cmd = {0};
    cmd.type = CMD_KEYBOARD;
    cmd.param.keyboard.mods = report->mods;
    for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
        cmd.param.keyboard.keys[i] = report->raw[i];
    }
    uart_cmd_send(&cmd);
}
