/**
 * @file flash.h
 * @author 
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

extern uint32_t flash_base_address;
extern uint32_t flash_total_size;
extern uint32_t flash_unit_size;

void flash_unlock(void);
void flash_lock(void);
bool flash_write(uint32_t address, uint16_t offset, uint16_t data);
void flash_read(uint32_t address, uint16_t *offset, uint16_t *data);
void flash_erase_pages(void);