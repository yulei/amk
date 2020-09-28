/**
 * @file ble_bas_service.h
 * @brief ble battery service
 */

#pragma once

#include "ble_config.h"

void ble_bas_service_init(void);
void ble_bas_service_start(void);
void ble_bas_service_prepare_sleep(void);