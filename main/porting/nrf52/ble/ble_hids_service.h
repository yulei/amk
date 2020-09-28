/**
 * @file ble_hids_service.h
 * @brief defined the hid service related part
 */

#pragma once

#include "ble_config.h"

void ble_hids_service_init(void);
void ble_hids_service_start(void);
void ble_hids_service_prepare_sleep(void);

void ble_hids_service_send_report(uint8_t report_id, uint8_t* report_data);
void ble_hids_service_flush(bool send);
