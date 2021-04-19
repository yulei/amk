/**
 * mb85rcxx.h
 *  currently supported MB85RC16
 */

#pragma once

#include <stdint.h>

void mb85rcxx_write_byte(uint16_t addr, uint8_t data);
uint8_t mb85rcxx_read_byte(uint16_t addr);

