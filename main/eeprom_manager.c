/**
 * eeprom_manager.c
 */

#include "eeprom_manager.h"
#include "eeconfig.h"
#include "flash_store.h"

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

void ee_keymap_write(uint8_t layer, const void* keymaps, size_t size)
{
    flash_store_write(layer, keymaps, size);
}

void ee_keymap_read(uint8_t layer, void* keymaps, size_t size)
{
    flash_store_read(layer, keymaps, size);
}