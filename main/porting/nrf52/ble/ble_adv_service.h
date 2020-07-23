/**
 * @file ble_adv_service.h
 * @brief ble advertising service
 */

#pragma once

#include "ble_config.h"

void ble_adv_service_init(void);
void ble_adv_service_start(bool erase_bonds);
