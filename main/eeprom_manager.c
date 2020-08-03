/**
 * eeprom_manager.c
 */

#include "eeprom_manager.h"
#include "eeconfig.h"

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