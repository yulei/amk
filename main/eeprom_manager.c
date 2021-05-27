/**
 * eeprom_manager.c
 */

#include "eeprom_manager.h"
#include "eeconfig.h"
#include "rgb_effects.h"
#include "rgb_matrix.h"
#include "amk_keymap.h"
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

uint8_t eeconfig_read_layout_options(void)
{
    return eeprom_read_byte(EECONFIG_LAYOUT_OPTIONS);
}

void eeconfig_write_layout_options(uint8_t options)
{
    eeprom_write_byte(EECONFIG_LAYOUT_OPTIONS, options);
}

void eeconfig_update_layout_options(uint8_t options)
{
    eeconfig_write_layout_options(options);
}

uint8_t eeconfig_read_device(void)
{
    return eeprom_read_byte(EECONFIG_DEVICE);
}

void eeconfig_write_device(uint8_t device)
{
    eeprom_write_byte(EECONFIG_DEVICE, device);
}

void eeconfig_update_device(uint8_t device)
{
    eeconfig_write_device(device);
}

#define KEYMAP_MAGIC_VALUE      0x4D585438
#define KEYMAP_MAGIC_DEFAULT    0xFFFFFFFF
#define KEYMAP_MAX_LAYER        4
#define KEYMAP_LAYER_SIZE      (MATRIX_ROWS*MATRIX_COLS*2)

static uint16_t* ee_keymap_get_addr(uint8_t layer, uint8_t row, uint8_t col)
{
    return (uint16_t*)(EEKEYMAP_START_ADDR + ((layer*KEYMAP_LAYER_SIZE)+(MATRIX_COLS*row+col)*2));
}

static uint8_t *ee_keymap_get_addr_by_offset(uint16_t offset)
{
    return (uint8_t*)(EEKEYMAP_START_ADDR + offset);
}

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

void ee_keymap_write_key(uint8_t layer, uint8_t row, uint8_t col, uint16_t key)
{
    uint16_t *addr = ee_keymap_get_addr(layer, row, col);
    eeprom_write_word(addr, key);
}

uint16_t ee_keymap_read_key(uint8_t layer, uint8_t row, uint8_t col)
{
    uint16_t *addr = ee_keymap_get_addr(layer, row, col);
    return eeprom_read_word(addr);
}

void ee_keymap_write_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    uint8_t *addr = ee_keymap_get_addr_by_offset(offset);
    for (int i = 0; i < size; i++) {
        *data = eeprom_read_byte(addr);
        data++;
        addr++;
    }
}
void ee_keymap_read_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    uint8_t *addr = ee_keymap_get_addr_by_offset(offset);
    for (int i = 0; i < size; i++) {
        eeprom_write_byte(addr, *data);
        data++;
        addr++;
    }
}

/*****************/
/* TMK functions */
/*****************/

// platform dependent preparation
__attribute__((weak))
void eeconfig_init_prepare(void) {}

void eeconfig_init(void)
{
    eeconfig_init_prepare();

    eeprom_write_word(EECONFIG_MAGIC,          EECONFIG_MAGIC_NUMBER);
    eeprom_write_byte(EECONFIG_DEBUG,          0);
    eeprom_write_byte(EECONFIG_DEFAULT_LAYER,  0);
    eeprom_write_byte(EECONFIG_KEYMAP,         0);
    eeprom_write_byte(EECONFIG_MOUSEKEY_ACCEL, 0);

#ifdef RGB_EFFECTS_ENABLE
    extern void effects_update_default(void);
    effects_update_default();
#endif

#ifdef RGB_MATRIX_ENABLE
    extern void rgb_matrix_update_default(void);
    rgb_matrix_update_default();
#endif

    eeprom_write_byte(EECONFIG_LAYOUT_OPTIONS, 0);
    eeprom_write_byte(EECONFIG_DEVICE, 0);

    // reset keymap to original state
    amk_keymap_reset();
}

void eeconfig_enable(void)
{
    eeprom_write_word(EECONFIG_MAGIC, EECONFIG_MAGIC_NUMBER);
}

void eeconfig_disable(void)
{
    eeprom_write_word(EECONFIG_MAGIC, 0xFFFF);
}

bool eeconfig_is_enabled(void)
{
    return (eeprom_read_word(EECONFIG_MAGIC) == EECONFIG_MAGIC_NUMBER);
}

uint8_t eeconfig_read_debug(void)      { return eeprom_read_byte(EECONFIG_DEBUG); }
void eeconfig_write_debug(uint8_t val) { eeprom_write_byte(EECONFIG_DEBUG, val); }

uint8_t eeconfig_read_default_layer(void)      { return eeprom_read_byte(EECONFIG_DEFAULT_LAYER); }
void eeconfig_write_default_layer(uint8_t val) { eeprom_write_byte(EECONFIG_DEFAULT_LAYER, val); }

uint8_t eeconfig_read_keymap(void)      { return eeprom_read_byte(EECONFIG_KEYMAP); }
void eeconfig_write_keymap(uint8_t val) { eeprom_write_byte(EECONFIG_KEYMAP, val); }
