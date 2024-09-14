/**
 * @file cy8cmbr3.h
 * @brief driver for cy8cmbr3xxxx
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "amk_hal.h"
#include "amk_error.h"

bool cy8cmbr3_init(void);

bool cy8cmbr3_read(uint16_t* port);

void cy8cmbr3_reset(void);

void cy8cmbr3_shutdown(void);