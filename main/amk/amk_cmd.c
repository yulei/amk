/**
 * @file amk_cmd.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "amk_cmd.h"
#include "amk_printf.h"

#ifndef CMD_DEBUG
#define CMD_DEBUG   0
#endif

#if CMD_DEBUG
#define cmd_debug  amk_printf
#else
#define cmd_debug(...)
#endif

#define BUF_SIZE    16
#define NAME_MAX    16
#define VALUE_MAX   96

typedef struct {
    uint32_t type;
    const char* str;
} cmd_str_t;

static cmd_str_t cmd_strs[] = {
    {CMD_KEYBOARD, CMD_KEYBOARD_STR},
    {CMD_MOUSE, CMD_MOUSE_STR},
    {CMD_NKRO, CMD_NKRO_STR},
    {CMD_SYSTEM, CMD_SYSTEM_STR},
    {CMD_CONSUMER, CMD_CONSUMER_STR},
    {CMD_SCREEN, CMD_SCREEN_STR},
    {CMD_LED, CMD_LED_STR},
    {CMD_LAYER, CMD_LAYER_STR},
    {CMD_KEYHIT, CMD_KEYHIT_STR},
    {CMD_STATUS, CMD_STATUS_STR},
    {CMD_TOUCH, CMD_TOUCH_STR},
    {CMD_TEXT, CMD_TEXT_STR},
    {CMD_TIME, CMD_TIME_STR},
    {CMD_TYPE_MAX, NULL},
};

static int32_t cmd_parse_type(const void *data, uint32_t size, uint8_t *type);
static int32_t cmd_parse_param(const void *data, uint32_t size, cmd_t* cmd);

int32_t cmd_parse(const void* data, uint32_t size, cmd_t* cmd)
{
    const uint8_t *cur = (const uint8_t*)data;
    if (size < 2) {
        //cmd_debug("INVALID command!!!! \n");
        return -1;
    }
    cmd_debug("%s\n", cur);

    cmd->type = CMD_TYPE_MAX;
    int32_t cmd_parsed = cmd_parse_type(cur, size, &cmd->type);

    if (cmd_parsed > 0) {
        int32_t param_parsed = cmd_parse_param(cur+cmd_parsed, size-cmd_parsed, cmd); 
        if (param_parsed > 0) {
            return cmd_parsed+param_parsed;
        }
    }

    return -1;
}

static int32_t cmd_parse_type(const void *data, uint32_t size, uint8_t *type)
{
    const uint8_t *cur = (const uint8_t *)data;
    char buf[BUF_SIZE];
    uint8_t index = 0;
    uint32_t buf_max = size > BUF_SIZE ? BUF_SIZE : size;

    while(index < buf_max) {
        if(cur[index] == CMD_DELI) {
            buf[index] = '\0';
            for (int i = 0; i < CMD_TYPE_MAX; i++) {
                if (!cmd_strs[i].str) break;

                if (strcmp(buf, cmd_strs[i].str) == 0) {
                    *type = cmd_strs[i].type;
                    return index+1;
                }
            }
            return -1;
        } else {
            buf[index] = cur[index];
            index++;
        }
    }

    return -1;
}

static int32_t cmd_parse_param_token(const void* data, uint32_t size, char* name, char* value)
{
    const char *cur = (const char*)data;
    char* endl = memchr(data, CMD_PARAM_ENDL, size);
    if (!endl) return -1;

    char* deli = memchr(data, CMD_PARAM_DELI, endl-cur);
    if (!deli) {
        if (cur == endl) {
            endl++;
            //amk_printf("Only ;\n");
        }
        // no value
        memcpy(name, cur, endl-cur);
        name[endl-cur] = '\0';
    } else {
        // only deli
        if ((endl-cur) == 1) {
            *name = *cur;
            name[1] = '\0';
            //amk_printf("Only =\n");
        } else {
            // has value
            memcpy(name, cur, deli-cur);
            name[deli-cur] = '\0';
            memcpy(value, deli+1, endl-deli-1);
            value[endl-deli-1] = '\0';
        }
    }

    return (endl-cur+1);
}

static void cmd_process_keyboard_param(const char *name, const char *value, cmd_t *cmd)
{
    if (strcmp(name, KEYBOARD_PARAM_MODS) == 0) {
        int d = strtol(value, NULL, 16);
        cmd->param.keyboard.mods = d;
    } else {
        int d = strtol(name, NULL, 16);
        if (d != 0) {
            for (int i = 0; i < KEYBOARD_KEY_SIZE; i++) {
                if (cmd->param.keyboard.keys[i] == 0) {
                    cmd->param.keyboard.keys[i] = d;
                    break;
                }
            }
        }
    }
}

static int32_t cmd_process_mouse_param(void)
{
    return -1;
}

static int32_t cmd_process_nkro_param(void)
{
    return -1;
}

static int32_t cmd_process_system_param(void)
{
    return -1;
}

static int32_t cmd_process_consumer_param(void)
{
    return -1;
}

static void cmd_process_layer_param(const char *name, const char *value, cmd_t *cmd)
{
    cmd->param.layer = strtol(name, NULL, 10);
}

static void cmd_process_led_param(const char *name, const char *value, cmd_t *cmd)
{
    cmd->param.led = strtol(name, NULL, 10);
}

static void cmd_process_keyhit_param(const char *name, const char *value, cmd_t *cmd)
{
    if (strcmp(name, KEYHIT_PARAM_ROW) == 0) {
        cmd->param.keyhit.row = strtol(value, NULL, 10);
    } else if (strcmp(name, KEYHIT_PARAM_COL) == 0) {
        cmd->param.keyhit.col = strtol(value, NULL, 10);
    } else if (strcmp(name, KEYHIT_PARAM_PRESSED) == 0) {
        cmd->param.keyhit.pressed = strtol(value, NULL, 10);
    }
}

static void cmd_process_screen_param(const char *name, const char *value, cmd_t *cmd)
{
    if (strcmp(name, SCREEN_PARAM_POWER) == 0) {
        cmd->param.screen.action = CMD_SCREEN_POWER;
        if (strcmp(value, CMD_PARAM_ON) == 0) {
            cmd->param.screen.state = 1;
        } else {
            cmd->param.screen.state = 0;
        }
    } else if (strcmp(name, SCREEN_PARAM_MSC) == 0) {
        cmd->param.screen.action = CMD_SCREEN_MSC;
        if (strcmp(value, CMD_PARAM_ON) == 0) {
            cmd->param.screen.state = 1;
        } else {
            cmd->param.screen.state = 0;
        }
    } else if (strcmp(name, SCREEN_PARAM_MODE) == 0)  {
        cmd->param.screen.action = CMD_SCREEN_MODE;
        cmd->param.screen.state = strtol(value, NULL, 10);
    } else if (strcmp(name, SCREEN_PARAM_ADJUST) == 0) {
        cmd->param.screen.action = CMD_SCREEN_ADJUST;
        cmd->param.screen.state = strtol(value, NULL, 10);
    } else if (strcmp(name, SCREEN_PARAM_DISPLAY) == 0) {
        cmd->param.screen.action = CMD_SCREEN_DISPLAY;
        cmd->param.screen.state = strtol(value, NULL, 10);
    }
}

static void cmd_process_status_param(const char *name, const char *value, cmd_t *cmd)
{
    if (strcmp(name, CMD_PARAM_OK) == 0) {
        cmd->param.status = 1;
    } else {
        cmd->param.status = 0;
    }
}

static void cmd_process_touch_param(const char *name, const char *value, cmd_t *cmd)
{
    int index = strtol(name, NULL, 16);
    int data = strtol(value, NULL, 16);
    if (index < TOUCH_PARAM_SIZE) {
        cmd->param.touch_keys[index] = data;
    } else {
        cmd_debug("CMD: invalid touch index=%d, value=%d\n", index, data);
    }
}

static void cmd_process_text_param(const char *name, const char *value, cmd_t *cmd)
{
    if (strcmp(name, CMD_PARAM_OFF) == 0) {
        for (int i = 0; i < TEXT_PARAM_SIZE; i++) {
            cmd->param.text[i] = 0;
        }
        cmd_debug("CMD: TEXT OFF\n");
        return;
    }

    for (int i = 0; i < TEXT_PARAM_SIZE; i++) {
        if (cmd->param.text[i] == 0) {
            cmd->param.text[i] = *name;
            cmd_debug("CMD: TEXT added %c\n", cmd->param.text[i]);
            return;
        }
    }

    cmd_debug("CMD: too many character in text\n");
}

static void cmd_process_time_param(const char *name, const char *value, cmd_t *cmd)
{
    if (strcmp(name, TIME_PARAM_YEAR) == 0) {
        cmd->param.time.year = strtol(value, NULL, 10);
    } else if (strcmp(name, TIME_PARAM_MONTH) == 0) {
        cmd->param.time.month = strtol(value, NULL, 10);
    } else if (strcmp(name, TIME_PARAM_DAY) == 0) {
        cmd->param.time.day = strtol(value, NULL, 10);
    } else if (strcmp(name, TIME_PARAM_WEEKDAY) == 0) {
        cmd->param.time.weekday = strtol(value, NULL, 10);
    } else if (strcmp(name, TIME_PARAM_HOUR) == 0) {
        cmd->param.time.hour = strtol(value, NULL, 10);
    } else if (strcmp(name, TIME_PARAM_MINUTE) == 0) {
        cmd->param.time.minute = strtol(value, NULL, 10);
    } else if (strcmp(name, TIME_PARAM_SECOND) == 0) {
        cmd->param.time.second = strtol(value, NULL, 10);
    } else {
        cmd_debug("CMD: TIME unknown name(%s), value(%s)\n", name, value);
    }
}

static int32_t cmd_parse_param(const void *data, uint32_t size, cmd_t* cmd)
{
    char name[NAME_MAX];
    char value[VALUE_MAX];
    uint32_t parsed = 0;
    const char* cur = (const char*)data;
    while( parsed < size) {
        int32_t result = cmd_parse_param_token(cur, size, name, value);
        if (result < 0) {
            break;
        } else {
            parsed += result;
            cur += result;
            switch(cmd->type) {
            case CMD_KEYBOARD:
                cmd_process_keyboard_param(name, value, cmd);
                break;
            case CMD_MOUSE:
                cmd_process_mouse_param();
                break;
            case CMD_NKRO:
                cmd_process_nkro_param();
                break;
            case CMD_SYSTEM:
                cmd_process_system_param();
                break;
            case CMD_CONSUMER:
                cmd_process_consumer_param();
                break;
            case CMD_SCREEN:
                cmd_process_screen_param(name, value, cmd);
                break;
            case CMD_LED:
                cmd_process_led_param(name, value, cmd);
                break;
            case CMD_LAYER:
                cmd_process_layer_param(name, value, cmd);
                break;
            case CMD_KEYHIT:
                cmd_process_keyhit_param(name, value, cmd);
                break;
            case CMD_STATUS:
                cmd_process_status_param(name, value, cmd);
                break;
            case CMD_TOUCH:
                cmd_process_touch_param(name, value, cmd);
                break;
            case CMD_TEXT:
                cmd_process_text_param(name, value, cmd);
                break;
            case CMD_TIME:
                cmd_process_time_param(name, value, cmd);
                break;
            default:
                cmd_debug("CMD: unkown type=%d\n", cmd->type);
                break;
            }
        }
    }
    
    if (parsed) return parsed;

    return -1;
}

static int32_t cmd_compose_keyboard(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t valid = 0;
    char *p = (char *)buf;
    // put modifiers always
    int32_t result = snprintf(p, size, "%s:%s=%x;",
                        CMD_KEYBOARD_STR,
                        KEYBOARD_PARAM_MODS,
                        cmd->param.keyboard.mods);

    if (result < 0) return result;
    valid += result;
    p += result;
    size -= result;
    for(int i = 1; i < KEYBOARD_KEY_SIZE; i++) {
        if (cmd->param.keyboard.keys[i]) {
            result = snprintf(p, size, "%x;", cmd->param.keyboard.keys[i]);
            if (result < 0) return result;

            valid += result;
            p += result;
            size -= result;
        }
    }

    result = snprintf(p, size, "\n");
    if (result < 0) return result;

    return result+valid;
}

static int32_t cmd_compose_screen(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t result = -1;
    if (cmd->param.screen.action == CMD_SCREEN_POWER) {
        result = snprintf(buf, size, "%s:%s=%s;\n", 
                    CMD_SCREEN_STR, 
                    SCREEN_PARAM_POWER,
                    cmd->param.screen.state ? CMD_PARAM_ON:CMD_PARAM_OFF);
    } else if (cmd->param.screen.action == CMD_SCREEN_MSC) {
        result = snprintf(buf, size, "%s:%s=%s;\n", 
                    CMD_SCREEN_STR, 
                    SCREEN_PARAM_MSC,
                    cmd->param.screen.state ? CMD_PARAM_ON:CMD_PARAM_OFF);
    } else if (cmd->param.screen.action == CMD_SCREEN_MODE) {
        result = snprintf(buf, size, "%s:%s=%d;\n", 
                    CMD_SCREEN_STR, 
                    SCREEN_PARAM_MODE,
                    cmd->param.screen.state);
    } else if (cmd->param.screen.action == CMD_SCREEN_ADJUST) {
        result = snprintf(buf, size, "%s:%s=%d;\n", 
                    CMD_SCREEN_STR, 
                    SCREEN_PARAM_ADJUST,
                    cmd->param.screen.state);
    } else if (cmd->param.screen.action == CMD_SCREEN_DISPLAY) {
        result = snprintf(buf, size, "%s:%s=%d;\n", 
                    CMD_SCREEN_STR, 
                    SCREEN_PARAM_DISPLAY,
                    cmd->param.screen.state);
    }

    return result;
}

static int32_t cmd_compose_led(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t result = snprintf(buf, size, "%s:%d;\n", 
                    CMD_LED_STR, 
                    cmd->param.led);

    return result;
}

static int32_t cmd_compose_layer(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t result = snprintf(buf, size, "%s:%d;\n", 
                    CMD_LAYER_STR, 
                    cmd->param.layer);

    return result;
}
static int32_t cmd_compose_keyhit(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t result = snprintf(buf, size, "%s:%s=%d;%s=%d;%s=%d;\n", 
                    CMD_KEYHIT_STR, 
                    KEYHIT_PARAM_ROW,
                    cmd->param.keyhit.row,
                    KEYHIT_PARAM_COL,
                    cmd->param.keyhit.col,
                    KEYHIT_PARAM_PRESSED,
                    cmd->param.keyhit.pressed);

    return result;
}

static int32_t cmd_compose_status(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t result = snprintf(buf, size, "%s:%s;\n", 
                    CMD_STATUS_STR, 
                    cmd->param.status ? CMD_PARAM_OK:CMD_PARAM_FAIL);

    return result;
}

static int32_t cmd_compose_touch(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t valid = 0;
    char *p = (char *)buf;
    // put cmd prefix
    int32_t result = snprintf(p, size, "%s:", CMD_TOUCH_STR);
    if (result < 0) return result;

    valid += result;
    p += result;
    size -= result;
    for(int i = 0; i < TOUCH_PARAM_SIZE; i++) {
        result = snprintf(p, size, "%x=%x;", i, cmd->param.touch_keys[i]);
        if (result < 0) return result;

        valid += result;
        p += result;
        size -= result;
    }

    result = snprintf(p, size, "\n");
    if (result < 0) return result;

    return result+valid;
}

static int32_t cmd_compose_text(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t valid = 0;
    char *p = (char *)buf;
    // put cmd prefix
    int32_t result = snprintf(p, size, "%s:", CMD_TEXT_STR);
    if (result < 0) return result;

    valid += result;
    p += result;
    size -= result;
    bool has_char = false;
    for(int i = 0; i < TEXT_PARAM_SIZE; i++) {
        if (cmd->param.text[i] != 0) {
            result = snprintf(p, size, "%c;", cmd->param.text[i]);
            if (result < 0) return result;
            valid += result;
            p += result;
            size -= result;
            has_char = true;
        }
    }

    if (!has_char) {
        result = snprintf(p, size, "%s;\n", CMD_PARAM_OFF);
    } else {
        result = snprintf(p, size, "\n");
    }

    if (result < 0) return result;

    return result+valid;
}

static int32_t cmd_compose_time(const cmd_t *cmd, void *buf, uint32_t size)
{
    int32_t valid = 0;
    char *p = (char *)buf;
    // put cmd prefix
    int32_t result = snprintf(p, size, "%s:", CMD_TIME_STR);
    if (result < 0) return result;

    valid += result;
    p += result;
    size -= result;

    result = snprintf(p, size, "%s=%d;%s=%d;%s=%d;%s=%d;%s=%d;%s=%d;%s=%d;",
                        TIME_PARAM_YEAR, cmd->param.time.year,
                        TIME_PARAM_MONTH, cmd->param.time.month,
                        TIME_PARAM_DAY, cmd->param.time.day,
                        TIME_PARAM_WEEKDAY, cmd->param.time.weekday,
                        TIME_PARAM_HOUR, cmd->param.time.hour,
                        TIME_PARAM_MINUTE, cmd->param.time.minute,
                        TIME_PARAM_SECOND, cmd->param.time.second);

    if (result < 0) return result;
    valid += result;
    p += result;
    size -= result;

    result = snprintf(p, size, "\n");
    if (result < 0) return result;

    return result+valid;
}

int32_t cmd_compose(const cmd_t *cmd, void* buf, uint32_t size)
{
    int32_t result = -1;
    switch(cmd->type) {
    case CMD_KEYBOARD:
        result = cmd_compose_keyboard(cmd, buf, size);
        break;
    case CMD_MOUSE:
        break;
    case CMD_NKRO:
        break;
    case CMD_SYSTEM:
        break;
    case CMD_CONSUMER:
        break;
    case CMD_SCREEN:
        result = cmd_compose_screen(cmd, buf, size);
        break;
    case CMD_LED:
        result = cmd_compose_led(cmd, buf, size);
        break;
    case CMD_LAYER:
        result = cmd_compose_layer(cmd, buf, size);
        break;
    case CMD_KEYHIT:
        result = cmd_compose_keyhit(cmd, buf, size);
        break;
    case CMD_STATUS:
        result = cmd_compose_status(cmd, buf, size);
        break;
    case CMD_TOUCH:
        result = cmd_compose_touch(cmd, buf, size);
        break;
    case CMD_TEXT:
        result = cmd_compose_text(cmd, buf, size);
        break;
    case CMD_TIME:
        result = cmd_compose_time(cmd, buf, size);
        break;
    default:
        break;
    }

    return result;
}
