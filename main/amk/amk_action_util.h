/**
 * @file amk_action_util.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "action_util.h"
#include "report.h"
#include "usb_common.h"

#define AMK_NKRO_REPORT_SIZE    (AMK_NKRO_TOTAL_SIZE-1)
#define AMK_NKRO_BITS_SIZE      (AMK_NKRO_TOTAL_SIZE-2)

typedef union {
    uint8_t raw[AMK_NKRO_REPORT_SIZE];
    struct {
        uint8_t mods;
        uint8_t bits[AMK_NKRO_BITS_SIZE];
    };
}__attribute__ ((packed)) amk_nkro_t;

typedef struct {
    union {
        report_keyboard_t   std;
        amk_nkro_t          nkro;
    };
    bool nkro_mode;
}__attribute__ ((packed)) amk_report_t;
