/**
 * @file amk_store.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "amk_hal.h"
#include "eeprom.h"
#include "amk_store.h"
#include "amk_printf.h"

#ifdef EC_MATRIX_ENABLE
#define AMK_STORE_APC_DEFAULT   20
#define AMK_STORE_RT_DEFAULT    0
#else
#define AMK_STORE_APC_DEFAULT   160
#define AMK_STORE_RT_DEFAULT    0 
#endif

void amk_store_set_apc(uint8_t profile, uint8_t row, uint8_t col, uint16_t apc)
{
    uint8_t *addr = (uint8_t *) ((MATRIX_ROWS*MATRIX_COLS*2*profile) + (row*MATRIX_COLS + col)*2 + AMK_STORE_APC_START);
    eeprom_write_byte(addr, apc >> 8);
    eeprom_write_byte(addr+1, apc & 0xFF);
}

uint16_t amk_store_get_apc(uint8_t profile, uint8_t row, uint8_t col)
{
    uint8_t *addr = (uint8_t *) ((MATRIX_ROWS*MATRIX_COLS*2*profile) + (row*MATRIX_COLS + col)*2 + AMK_STORE_APC_START);
    uint16_t high = eeprom_read_byte(addr);
    uint16_t low = eeprom_read_byte(addr+1);
    uint16_t apc = (high << 8) | low;
    if (apc == 0) {
        apc = AMK_STORE_APC_DEFAULT;
    }
    return apc;
}

void amk_store_set_rt(uint8_t profile, uint8_t row, uint8_t col, uint16_t rt)
{
    uint8_t *addr = (uint8_t *) ((MATRIX_ROWS*MATRIX_COLS*2*profile) + (row*MATRIX_COLS + col)*2 + AMK_STORE_RT_START);
    eeprom_write_byte(addr, rt >> 8);
    eeprom_write_byte(addr+1, rt& 0xFF);
}

uint16_t amk_store_get_rt(uint8_t profile, uint8_t row, uint8_t col)
{
    uint8_t *addr = (uint8_t *) ((MATRIX_ROWS*MATRIX_COLS*2*profile) + (row*MATRIX_COLS + col)*2 + AMK_STORE_RT_START);
    uint16_t high = eeprom_read_byte(addr);
    uint16_t low = eeprom_read_byte(addr+1);

    return (high << 8) | low;
}

void amk_store_set_dks(uint8_t row, uint8_t col, void* dks)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*sizeof(struct amk_dks) + AMK_STORE_DKS_START);
    struct amk_dks* p = (struct amk_dks*)dks;
    for (int i = 0; i < AMK_DKS_EVENT_MAX; i++) {
        eeprom_write_byte(addr++, p->events[i]);
    }
    for (int j = 0; j < AMK_DKS_KEY_MAX; j++) {
        uint8_t high = (uint8_t)(p->keys[j] >> 8);
        uint8_t low = (uint8_t)(p->keys[j] & 0xFF);
        eeprom_write_byte(addr++, high);
        eeprom_write_byte(addr++, low);
    }
}

void amk_store_get_dks(uint8_t row, uint8_t col, void* dks)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*sizeof(struct amk_dks) + AMK_STORE_DKS_START);
    struct amk_dks* p = (struct amk_dks*)dks;
    for (int i = 0; i < AMK_DKS_EVENT_MAX; i++) {
        p->events[i] = eeprom_read_byte(addr++);
    }
    for (int j = 0; j < AMK_DKS_KEY_MAX; j++) {
        uint16_t high = eeprom_read_byte(addr++);
        uint16_t low = eeprom_read_byte(addr++);

        p->keys[j] = (high << 8) | low;
    }
}

void amk_store_set_snaptap(uint8_t index, void* snaptap)
{
    uint8_t *addr = (uint8_t *) (index*sizeof(struct amk_snaptap) + AMK_STORE_SNAPTAP_START);
    struct amk_snaptap* p = (struct amk_snaptap*)snaptap;
    eeprom_write_byte(addr++, p->first_row);
    eeprom_write_byte(addr++, p->first_col);
    eeprom_write_byte(addr++, p->second_row);
    eeprom_write_byte(addr++, p->second_col);
    eeprom_write_byte(addr++, p->mode);
}

void amk_store_get_snaptap(uint8_t index, void* snaptap)
{
    uint8_t *addr = (uint8_t *) (index*sizeof(struct amk_snaptap) + AMK_STORE_SNAPTAP_START);
    struct amk_snaptap* p = (struct amk_snaptap*)snaptap;
    p->first_row = eeprom_read_byte(addr++);
    p->first_col = eeprom_read_byte(addr++);
    p->second_row = eeprom_read_byte(addr++);
    p->second_col = eeprom_read_byte(addr++);
    p->mode = eeprom_read_byte(addr++);
}

void amk_store_set_snaptap_config(uint8_t config)
{
    uint8_t *addr = (uint8_t *) (AMK_STORE_SNAPTAP_CONFIG_START);
    eeprom_write_byte(addr, config);
}

uint8_t amk_store_get_snaptap_config()
{
    uint8_t *addr = (uint8_t *) (AMK_STORE_SNAPTAP_CONFIG_START);
    return eeprom_read_byte(addr);
}

#ifdef RGB_ENABLE
void amk_store_set_led(uint8_t index, const struct amk_led* led)
{
    uint8_t *addr = (uint8_t*)(index*sizeof(struct amk_led) + AMK_STORE_LED_START);
    eeprom_write_byte(addr++, led->hue);
    eeprom_write_byte(addr++, led->sat);
    eeprom_write_byte(addr++, led->val);
    eeprom_write_byte(addr++, led->param);
}

void amk_store_get_led(uint8_t index, struct amk_led* led)
{
    uint8_t *addr = (uint8_t*)(index*sizeof(struct amk_led) + AMK_STORE_LED_START);
    led->hue = eeprom_read_byte(addr++);
    led->sat = eeprom_read_byte(addr++);
    led->val = eeprom_read_byte(addr++);
    led->param = eeprom_read_byte(addr++);
}
#endif

void amk_store_init(void)
{
    uint8_t dks[sizeof(struct amk_dks)] = {[0 ... sizeof(struct amk_dks)-1] = 0};
    for (int profile = 0; profile < AMK_STORE_APCRT_PROFILE_COUNT; profile++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                amk_store_set_apc(profile, row, col, AMK_STORE_APC_DEFAULT);
                amk_store_set_rt(profile, row, col, AMK_STORE_RT_DEFAULT);
            }
        }
    }
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            amk_store_set_dks(row, col, dks);
        }
    }
    #ifdef RGB_ENABLE
    struct amk_led led = {
        .hue = 0,
        .sat = 0xFF,
        .val = 0xFF,
        .on = 1,
        .dynamic = 0,
        .blink = 0,
        .breath = 0,
        .speed = 8,
    };
    for (int i = 0; i < RGB_LED_NUM; i++) {
        amk_store_set_led(i, &led);
    }
    #endif
}

void eeconfig_init_custom(void)
{
    amk_printf("AMK STORE init: apc:%d, rt:%d, dks:%d, snaptap%d-%d\n", 
            AMK_STORE_APC_START, AMK_STORE_RT_START, AMK_STORE_DKS_START, AMK_STORE_SNAPTAP_START,
            AMK_STORE_ANA_END);

    #ifdef RGB_ENABLE
    amk_printf("AMK STORE RGB: start:%d, end:%d\n", AMK_STORE_LED_START, AMK_STORE_LED_END);
    #endif

    amk_store_init();
}
