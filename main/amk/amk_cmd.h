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
static const char KEYBOARD_PARAM_MODS[] = "M";

enum {
    CMD_SCREEN_POWER,
    CMD_SCREEN_MSC,
};

#define KEYBOARD_KEY_SIZE       6
#define MOUSE_PARAM_SIZE        6
#define NKRO_PARAM_SIZE         32

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
