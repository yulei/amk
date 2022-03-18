/**
 * @file hid_report_parser.h
 *
 *  simple hid report descriptor parser, the main goal was support tmk/qmk based keyboard
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

//
// there will have 4 interfaces at maximum
//
// keybaord, mouse, extra(system & consumer), nkro
// in tmk: they will be list in order if the specific feature was opened
// in qmk: mouse&extra&nkro will share the same interface, and keyboard 
// interface will also share with them if KEYBOARD_SHARE_EP was enabled

// report descriptor item type
enum {
    RDI_NONE,
    RDI_KEYBOARD,       // boot keyboard
    RDI_MOUSE,          // boot mouse
    RDI_SYSTEM,         // system
    RDI_CONSUMER,       // consumer
    RDI_RAW,            // qmk raw
    RDI_MAX,
};

// interface type
enum {
    ITF_HID_UNKNOWN = 0,
    ITF_HID_MOUSE,
    ITF_HID_KEYBOARD,
    ITF_HID_EXTRA,
    ITF_HID_RAW,
};

enum {
    ORDER_MODIFIER,
    ORDER_RESERVED,
    ORDER_KEYCODE,
    ORDER_LED,
    ORDER_MAX,
};

#define ITF_MAX 4       // tmk could have keyboard, mouse, extra and nkro, qmk can have, keyboard, shared, raw
#define RD_MAX  5       // qmk can let keyboard, mouse, extra(system, consumer), nkro in one interface

typedef struct {
    uint8_t keycode_size;
    uint8_t order;
    uint8_t orders[ORDER_MAX];  // order of the modifier, led and keycodes
} keyboard_descriptor_t;

typedef struct {
    uint8_t wheel_enable;
} mouse_descriptor_t;

typedef struct {
    uint8_t keycode_size;
} system_descriptor_t;

typedef struct {
    uint8_t keycode_size;
} consumer_descriptor_t;

typedef struct {
    uint8_t input_size;
    uint8_t output_size;
} raw_descriptor_t;

typedef struct {
    union {
        keyboard_descriptor_t keyboard;
        mouse_descriptor_t mouse;
        system_descriptor_t system;
        consumer_descriptor_t consumer;
        raw_descriptor_t raw;
    };
    uint8_t report_id;
    uint8_t rd_type;
} report_descriptor_t;

typedef struct {
    uint8_t itf;
    uint8_t valid;
    uint8_t desc_count;
    report_descriptor_t desc[RD_MAX];
} itf_report_desc_t;

void rp_reset(void);
bool rp_parse(uint32_t itf, const void* data, uint32_t size);
itf_report_desc_t *rp_get(uint32_t itf);
