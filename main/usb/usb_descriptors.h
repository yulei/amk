/**
 *  usb_descriptors.h
 */ 

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "usb_common.h"

uint8_t const* tud_descriptor_device_cb(void);
uint32_t tud_descriptor_device_size(void);

uint8_t const* tud_descriptor_configuration_cb(uint8_t index);
uint32_t tud_descriptor_configuration_size(uint8_t index);

uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf);
uint32_t tud_hid_descriptor_report_size(uint8_t itf);

uint8_t const* tud_descriptor_hid_report_kbd_cb(void);
uint32_t tud_descriptor_hid_report_kbd_size(void);

uint8_t const* tud_descriptor_hid_report_other_cb(void);
uint32_t tud_descriptor_hid_report_other_size(void);

uint8_t const* tud_hid_descriptor_interface_cb(uint8_t itf);
uint32_t tud_hid_descriptor_interface_size(uint8_t itf);

uint8_t const* tud_descriptor_hid_interface_kbd_cb(void);
uint32_t tud_descriptor_hid_interface_kbd_size(void);

uint8_t const* tud_descriptor_hid_interface_other_cb(void);
uint32_t tud_descriptor_hid_interface_other_size(void);

#ifdef VIAL_ENABLE
uint8_t const* tud_descriptor_hid_report_vial_cb(void);
uint32_t tud_descriptor_hid_report_vial_size(void);

uint8_t const* tud_descriptor_hid_interface_vial_cb(void);
uint32_t tud_descriptor_hid_interface_vial_size(void);
#endif

uint8_t const* tud_descriptor_device_qualifier_cb(void);
uint32_t tud_descriptor_device_qualifier_size(void);

uint8_t const* tud_descriptor_other_speed_configuration_cb(uint8_t index);
uint32_t tud_descriptor_other_speed_configuration_size(uint8_t index);

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid);

uint8_t *get_descriptor_str(uint8_t index, uint16_t *length);
