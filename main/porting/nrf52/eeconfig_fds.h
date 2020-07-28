/**
 * @file eeprom_fds.h
 *
 */

#pragma once

#include <stdint.h>

void fds_eeprom_init(void);
void fds_eeprom_write_byte(uint16_t addr, uint8_t data);
uint8_t fds_eeprom_read_byte(uint16_t addr);
