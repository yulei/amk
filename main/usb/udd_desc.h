/**
 *  @file udd_desc.h
 */ 

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "usb_common.h"

uint8_t const* udd_descriptor_device(void);
uint32_t udd_descriptor_device_size(void);

uint8_t const* udd_descriptor_configuration(uint8_t index);
uint32_t udd_descriptor_configuration_size(uint8_t index);

uint8_t const* udd_hid_descriptor_report(uint8_t itf);
uint32_t udd_hid_descriptor_report_size(uint8_t itf);

uint8_t const* udd_descriptor_hid_report_kbd(void);
uint32_t udd_descriptor_hid_report_kbd_size(void);

uint8_t const* udd_descriptor_hid_report_other(void);
uint32_t udd_descriptor_hid_report_other_size(void);

uint8_t const* udd_hid_descriptor_interface(uint8_t itf);
uint32_t udd_hid_descriptor_interface_size(uint8_t itf);

uint8_t const* udd_descriptor_hid_interface_kbd(void);
uint32_t udd_descriptor_hid_interface_kbd_size(void);

uint8_t const* udd_descriptor_hid_interface_other(void);
uint32_t udd_descriptor_hid_interface_other_size(void);

#ifdef VIAL_ENABLE
uint8_t const* udd_descriptor_hid_report_vial(void);
uint32_t udd_descriptor_hid_report_vial_size(void);

uint8_t const* udd_descriptor_hid_interface_vial(void);
uint32_t udd_descriptor_hid_interface_vial_size(void);
#endif

uint16_t const* udd_descriptor_string(uint8_t index, uint16_t langid);

uint8_t *get_descriptor_str(uint8_t index, uint16_t *length);
