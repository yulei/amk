/** 
 * @file udd_desc_stub.c 
 */

#include <string.h>

#include "usb_desc_def.h"
#include "usb_descriptors.h"
#include "tusb.h"

uint8_t const* udd_descriptor_device(void)
{
    return tud_descriptor_device_cb();
}

uint32_t udd_descriptor_device_size(void)
{
    return tud_descriptor_device_size();
}
uint8_t const* udd_descriptor_hid_report_kbd(void)
{
    return tud_descriptor_hid_report_kbd_cb();
}

uint32_t udd_descriptor_hid_report_kbd_size(void)
{
    return tud_descriptor_hid_report_kbd_size();
}

uint8_t const* udd_descriptor_hid_report_other(void)
{
    return tud_descriptor_hid_report_other_cb();
}

uint32_t udd_descriptor_hid_report_other_size(void)
{
    return tud_descriptor_hid_report_other_size();
}

uint8_t const* udd_descriptor_configuration(uint8_t index)
{
    return tud_descriptor_configuration_cb(index);
}

uint32_t udd_descriptor_configuration_size(uint8_t index)
{
    uint32_t size = tud_descriptor_configuration_size(index);

    return size;
}

uint8_t const* udd_hid_descriptor_report(uint8_t itf)
{
    return tud_hid_descriptor_report_cb(itf);
}

uint32_t udd_hid_descriptor_report_size(uint8_t itf)
{
    return tud_hid_descriptor_report_size(itf);
}

uint32_t udd_hid_descriptor_interface_size(uint8_t itf)
{
    return tud_hid_descriptor_interface_size(itf);
}

uint8_t const* udd_descriptor_hid_interface_kbd(void)
{
    return tud_descriptor_hid_interface_kbd_cb();
}

uint32_t udd_descriptor_hid_interface_kbd_size(void)
{
    return tud_descriptor_hid_interface_kbd_size();
}

uint8_t const* udd_descriptor_hid_interface_other(void)
{
    return tud_descriptor_hid_interface_other_cb();
}

uint32_t udd_descriptor_hid_interface_other_size(void)
{
    return tud_descriptor_hid_interface_other_size();
}

uint16_t const* udd_descriptor_string(uint8_t index, uint16_t langid)
{
    return tud_descriptor_string_cb(index, langid);
}

#ifdef VIAL_ENABLE
uint8_t const* udd_descriptor_hid_report_vial(void)
{
    return tud_descriptor_hid_report_vial_cb();
}

uint32_t udd_descriptor_hid_report_vial_size(void)
{
    return tud_descriptor_hid_report_vial_size();
}

uint8_t const* udd_descriptor_hid_interface_vial(void)
{
    return tud_descriptor_hid_interface_vial_cb();
}

uint32_t udd_descriptor_hid_interface_vial_size(void)
{
    return tud_descriptor_hid_interface_vial_size();
}
#endif