/**
 * @file amk_macro.c
 */

#include "amk_macro.h"
#include "usb_descriptors.h"
#include "usb_interface.h"

void amk_macro_play(uint8_t id)
{
#ifdef VIAL_ENABLE
    amk_macro_t report;
    report.id = id;
    report.offset = 0;
    report.delay = 0;
    usb_send_report(HID_REPORT_ID_MACRO, &report, sizeof(report));
#endif
}