/**
 * @file ble_services.h
 * @brief ble services
 */

#pragma once

#include "ble_config.h"

void ble_services_init(void);
void ble_services_start(bool erase_bond);

void ble_pm_whitelist_set(pm_peer_id_list_skip_t skip);
void ble_pm_identities_set(pm_peer_id_list_skip_t skip);
void ble_pm_delete_bonds(void);
void ble_qwr_update_handle(uint16_t conn_handle);
