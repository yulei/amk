/**
 * @file amk_store.h
 * @brief store internal data on emulated eeprom
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#pragma once

#include "amk_eeprom.h"


#define AMK_STORE_START                 (AMK_EEPROM_SIZE-EECONFIG_RESERVED_COUNT)
#define AMK_STORE_APCRT_PROFILE_COUNT   4

#define AMK_STORE_APC_START (AMK_STORE_START)
void amk_store_set_apc(uint8_t profile, uint8_t row, uint8_t col, uint16_t apc);
uint16_t amk_store_get_apc(uint8_t profile, uint8_t row, uint8_t col);

#define AMK_STORE_RT_START  (AMK_STORE_APC_START+(MATRIX_ROWS*MATRIX_COLS*2)*AMK_STORE_APCRT_PROFILE_COUNT)
void amk_store_set_rt(uint8_t profile, uint8_t row, uint8_t col, uint16_t rt);
uint16_t amk_store_get_rt(uint8_t profile, uint8_t row, uint8_t col);

#define AMK_STORE_DKS_START (AMK_STORE_RT_START + (MATRIX_ROWS*MATRIX_COLS*2)*AMK_STORE_APCRT_PROFILE_COUNT)
#define AMK_DKS_EVENT_MAX   4
#define AMK_DKS_KEY_MAX     4
struct amk_dks {
    uint8_t     events[AMK_DKS_EVENT_MAX];
    uint16_t    keys[AMK_DKS_KEY_MAX];
} __attribute__((packed));

#define AMK_STORE_DKS_END   (AMK_STORE_DKS_START + MATRIX_ROWS*MATRIX_COLS*sizeof(struct amk_dks))

_Static_assert(AMK_STORE_DKS_END < AMK_EEPROM_SIZE, "EEPROM: dks out of range");

void amk_store_set_dks(uint8_t row, uint8_t col, void* dks);
void amk_store_get_dks(uint8_t row, uint8_t col, void* dks);

#ifdef RGB_ENABLE
#define AMK_STORE_LED_START (AMK_STORE_DKS_END)
struct amk_led {
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
    union {
        uint8_t param;
        struct {
            uint8_t on      : 1;
            uint8_t dynamic : 1;
            uint8_t blink   : 1;
            uint8_t breath  : 1;
            uint8_t speed   : 4;
        };
    };
} __attribute__((packed));

#define AMK_STORE_LED_COUNT RGB_LED_NUM
#define AMK_STORE_LED_END   (AMK_STORE_LED_START + AMK_STORE_LED_COUNT*sizeof(struct amk_led))
_Static_assert(AMK_STORE_LED_END < AMK_EEPROM_SIZE, "EEPROM: leds out of range");

void amk_store_set_led(uint8_t index, const struct amk_led* led);
void amk_store_get_led(uint8_t index, struct amk_led* led);
#endif

void amk_store_init(void);