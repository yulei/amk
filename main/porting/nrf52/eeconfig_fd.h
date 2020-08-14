/**
 * eeconfig_fd.h
 *  eeconfig emulation based on the flash storage
 */

#pragma once

#include <stdint.h>

void fd_eeprom_init(void);
void fd_eeprom_write_byte(uint16_t addr, uint8_t data);
uint8_t fd_eeprom_read_byte(uint16_t addr);
