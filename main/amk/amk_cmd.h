/**
 * @file amk_cmd.h
 * @author astro
 * 
 * @brief multiple mcus communication protocol, mimic the http protocol
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>

enum {
    CMD_KEYBOARD,
    CMD_MOUSE,
    CMD_NKRO,
    CMD_SYSTEM,
    CMD_CONSUMER,
    CMD_SCREEN,
    CMD_LED,
    CMD_LAYER,
    CMD_KEYHIT,
    CMD_STATUS,
    CMD_TOUCH,
    CMD_TEXT,
    CMD_TIME,
    CMD_TYPE_MAX,
};

#define CMD_DELI        ':'
#define CMD_PARAM_DELI  '='
#define CMD_PARAM_ENDL  ';'
#define CMD_ENDL        '\n'

static const char CMD_KEYBOARD_STR[] = "KBD";
static const char CMD_MOUSE_STR[] = "MS";
static const char CMD_NKRO_STR[] = "NKRO";
static const char CMD_SYSTEM_STR[] = "SYS";
static const char CMD_CONSUMER_STR[] = "CSR";
static const char CMD_SCREEN_STR[] = "SCR";
static const char CMD_STATUS_STR[] = "STS";
static const char CMD_LED_STR[] = "LED";
static const char CMD_LAYER_STR[] = "LYR";
static const char CMD_KEYHIT_STR[] = "KH";
static const char CMD_TOUCH_STR[] = "TC";
static const char CMD_TEXT_STR[] = "TX";
static const char CMD_TIME_STR[] = "TM";

static const char CMD_PARAM_OK[] = "OK";
static const char CMD_PARAM_FAIL[] = "FAIL";
static const char CMD_PARAM_ON[] = "ON";
static const char CMD_PARAM_OFF[] = "OFF";
static const char CMD_PARAM_YES[] = "YES";
static const char CMD_PARAM_NO[] = "NO";

static const char KEYHIT_PARAM_ROW[] = "ROW";
static const char KEYHIT_PARAM_COL[] = "COL";
static const char KEYHIT_PARAM_PRESSED[] = "PRESSED";
static const char SCREEN_PARAM_POWER[] = "PWR";
static const char SCREEN_PARAM_MSC[] = "MSC";
static const char SCREEN_PARAM_MODE[] = "MODE";
static const char SCREEN_PARAM_ADJUST[] = "ADJ";
static const char SCREEN_PARAM_DISPLAY[] = "DSP";
static const char KEYBOARD_PARAM_MODS[] = "M";
static const char TIME_PARAM_YEAR[] = "YR";
static const char TIME_PARAM_MONTH[] = "MH";
static const char TIME_PARAM_DAY[] = "DY";
static const char TIME_PARAM_WEEKDAY[] = "WY";
static const char TIME_PARAM_HOUR[] = "HR";
static const char TIME_PARAM_MINUTE[] = "ME";
static const char TIME_PARAM_SECOND[] = "SD";

enum {
    CMD_SCREEN_POWER,
    CMD_SCREEN_MSC,
    CMD_SCREEN_MODE,
    CMD_SCREEN_ADJUST,
    CMD_SCREEN_DISPLAY,
};

#define KEYBOARD_KEY_SIZE       6
#define MOUSE_PARAM_SIZE        6
#define NKRO_PARAM_SIZE         32

#ifndef TOUCH_PARAM_SIZE
#define TOUCH_PARAM_SIZE        8
#endif

#define TEXT_PARAM_SIZE         8

typedef struct {
    uint8_t type;
    union {
        struct {
            uint8_t action;
            uint8_t state;
        } screen;
        struct {
            uint8_t mods;
            uint8_t keys[KEYBOARD_KEY_SIZE];
        } keyboard;
        uint8_t mouse[MOUSE_PARAM_SIZE];
        uint8_t nkro[NKRO_PARAM_SIZE];
        uint16_t system;
        uint16_t consumer;
        uint8_t status;
        uint8_t led;
        uint8_t layer;
        struct {
            uint8_t row;
            uint8_t col;
            uint8_t pressed;
        } keyhit;
        uint8_t touch_keys[TOUCH_PARAM_SIZE];
        char text[TEXT_PARAM_SIZE];
        struct {
            uint16_t year;
            uint8_t month;
            uint8_t day;
            uint8_t weekday;
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } time;
    } param;
} cmd_t;

/**
 * @brief parse the command from other mcu
 * 
 * @return > 0 means parsed size, otherwise means error 
 */
int32_t cmd_parse(const void* data, uint32_t size, cmd_t* cmd);

/**
 * @brief compose the command to string
 * 
 * @return > size of the composed string, <= 0  means error 
 */
int32_t cmd_compose(const cmd_t *cmd, void* buf, uint32_t size);
