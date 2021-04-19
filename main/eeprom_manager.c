/**
 * eeprom_manager.c
 */

#include "eeprom_manager.h"
#include "eeconfig.h"
#include "flash_store.h"
#include "rgb_effects.h"
#include "rgb_matrix.h"
#include "amk_printf.h"

#ifndef EEPROM_MANAGER_DEBUG
#define EEPROM_MANAGER_DEBUG 1
#endif

#if EEPROM_MANAGER_DEBUG
#define ee_mgr_debug  amk_printf
#else
#define ee_mgr_debug(...)
#endif

uint32_t eeconfig_read_kb(void)
{
    return eeprom_read_dword(EECONFIG_KEYBOARD);
}

void eeconfig_write_kb(uint32_t data)
{
    eeprom_write_dword(EECONFIG_KEYBOARD, data);
}

void eeconfig_update_kb(uint32_t data)
{
    eeprom_update_dword(EECONFIG_KEYBOARD, data);
}

void eeconfig_read_rgb(void* rgb)
{
    rgb_effects_config_t *config = (rgb_effects_config_t*)rgb;
    config->enable  = eeprom_read_byte(EECONFIG_RGB);
    config->mode    = eeprom_read_byte(EECONFIG_RGB+1);
    config->speed   = eeprom_read_byte(EECONFIG_RGB+2);
    config->hue     = eeprom_read_byte(EECONFIG_RGB+3);
    config->sat     = eeprom_read_byte(EECONFIG_RGB+4);
    config->val     = eeprom_read_byte(EECONFIG_RGB+5);
    ee_mgr_debug("EE MGR: read rgb, enable: %d\n", config->enable);
}

void eeconfig_write_rgb(const void* rgb)
{
    rgb_effects_config_t *config = (rgb_effects_config_t*)rgb;
    eeprom_write_byte(EECONFIG_RGB, config->enable);
    eeprom_write_byte(EECONFIG_RGB+1, config->mode);
    eeprom_write_byte(EECONFIG_RGB+2, config->speed);
    eeprom_write_byte(EECONFIG_RGB+3, config->hue);
    eeprom_write_byte(EECONFIG_RGB+4, config->sat);
    eeprom_write_byte(EECONFIG_RGB+5, config->val);
    ee_mgr_debug("EE MGR: write rgb, enable: %d\n", config->enable);
}

void eeconfig_update_rgb(const void* rgb)
{
    eeconfig_write_rgb(rgb);
}

void eeconfig_read_rgb_matrix(void* rgb)
{
    rgb_matrix_config_t *config = (rgb_matrix_config_t*)rgb;
    config->enable  = eeprom_read_byte(EECONFIG_RGB_MATRIX);
    config->mode    = eeprom_read_byte(EECONFIG_RGB_MATRIX+1);
    config->speed   = eeprom_read_byte(EECONFIG_RGB_MATRIX+2);
    config->hue     = eeprom_read_byte(EECONFIG_RGB_MATRIX+3);
    config->sat     = eeprom_read_byte(EECONFIG_RGB_MATRIX+4);
    config->val     = eeprom_read_byte(EECONFIG_RGB_MATRIX+5);
}

void eeconfig_write_rgb_matrix(const void* rgb)
{
    rgb_matrix_config_t *config = (rgb_matrix_config_t*)rgb;
    eeprom_write_byte(EECONFIG_RGB_MATRIX, config->enable);
    eeprom_write_byte(EECONFIG_RGB_MATRIX+1, config->mode);
    eeprom_write_byte(EECONFIG_RGB_MATRIX+2, config->speed);
    eeprom_write_byte(EECONFIG_RGB_MATRIX+3, config->hue);
    eeprom_write_byte(EECONFIG_RGB_MATRIX+4, config->sat);
    eeprom_write_byte(EECONFIG_RGB_MATRIX+5, config->val);
}

void eeconfig_update_rgb_matrix(const void* rgb)
{
    eeconfig_write_rgb_matrix(rgb);
}

#define KEYMAP_MAGIC_VALUE      0x4D585438
#define KEYMAP_MAGIC_DEFAULT    0xFFFFFFFF
#define KEYMAP_MAX_LAYER        4


bool ee_keymap_is_valid(void)
{
    uint32_t magic = eeprom_read_dword(EECONFIG_KEYMAP_MAGIC);
    return (magic==KEYMAP_MAGIC_VALUE);
}

void ee_keymap_set_valid(bool valid)
{
    if (valid) {
        eeprom_write_dword(EECONFIG_KEYMAP_MAGIC, KEYMAP_MAGIC_VALUE);
    } else {
        eeprom_write_dword(EECONFIG_KEYMAP_MAGIC, KEYMAP_MAGIC_DEFAULT);
    }
}

void ee_keymap_write_layer(uint8_t layer, const void* keymaps, size_t size)
{
    flash_store_write(layer, keymaps, size);
}

void ee_keymap_read_layer(uint8_t layer, void* keymaps, size_t size)
{
    flash_store_read(layer, keymaps, size);
}

void ee_keymap_write_key(uint8_t layer, uint8_t row, uint8_t col, uint16_t key)
{
    flash_store_write_key(layer, row, col, key);
}

uint16_t ee_keymap_read_key(uint8_t layer, uint8_t row, uint8_t col)
{
    return flash_store_read_key(layer, row, col);
}