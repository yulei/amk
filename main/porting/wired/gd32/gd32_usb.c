/**
 * @file gd32_usb.c
 * 
 */

#include "amk_usb.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "drv_usb_hw.h"
#include "drv_usb_core.h"
#include "usbh_hid_core.h"
#include "amk_printf.h"

#include "report.h"
#include "wait.h"

#if USB_HOST_ENABLE
usbh_host usb_host;
static void usbh_user_init(void);
static void usbh_user_deinit(void);
static void usbh_user_device_connected(void);
static void usbh_user_device_reset(void);
static void usbh_user_device_disconnected(void);
static void usbh_user_over_current_detected(void);
static void usbh_user_device_speed_detected(uint32_t DeviceSpeed);
static void usbh_user_device_desc_available(void* desc);
static void usbh_user_device_address_assigned(void);
static void usbh_user_configuration_descavailable(usb_desc_config *cfgDesc, usb_desc_itf *itfDesc, usb_desc_ep *epDesc);
static void usbh_user_manufacturer_string(void* str);
static void usbh_user_product_string(void* str);
static void usbh_user_serialnum_string(void* str);
static void usbh_user_enumeration_finish(void);
static usbh_user_status usbh_user_userinput(void);
static int usbh_user_app(void);
static void usbh_user_device_not_supported(void);
static void usbh_user_unrecovered_error(void);

usbh_user_cb user_cb = {
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    usbh_user_app,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};
#endif

void amk_usb_init(void)
{
    usb_rcu_config();
    usb_vbus_config();
#ifdef USB_HOST_ENABLE
    usbh_class_register (&usb_host, &usbh_hid);
    usbh_init (&usb_host, &usr_cb);
    usb_intr_config ();
#endif
}

void amk_usb_task(void)
{
#ifdef USB_HOST_ENABLE
    usbh_core_task (&usb_host);
#endif
}

bool amk_usb_itf_ready(uint32_t type)
{
    return true;
}

bool amk_usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
    return true;
}

bool amk_usb_ready(void)
{
    return true;
}

bool amk_usb_suspended(void)
{
    return false;
}

void amk_usb_remote_wakeup(void)
{
}

void amk_usb_connect(bool on)
{
}

#ifdef USB_HOST_ENABLE

const uint8_t MSG_HOST_HEADER[] = "USB OTG FS HID Host";
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v2.1.0";

void usbh_user_init(void)
{
    static uint8_t startup = 0U;

    if(0U == startup){
        startup = 1U;
    }
}

void usbh_user_device_connected(void)
{
}

void usbh_user_unrecovered_error (void)
{
}

void usbh_user_device_disconnected (void)
{
}

void usbh_user_device_reset(void)
{
}

void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if (PORT_SPEED_HIGH == device_speed) {
    } else if(PORT_SPEED_FULL == device_speed) {
    } else if(PORT_SPEED_LOW == device_speed) {
    } else {
    }
}

void usbh_user_device_desc_available(void *device_desc)
{
    usb_desc_dev *pDevStr = device_desc;
}

void usbh_user_device_address_assigned(void)
{
}

void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc,
                                           usb_desc_itf *itf_desc,
                                           usb_desc_ep *ep_desc)
{
    usb_desc_itf *id = itf_desc;

    if (0x08U == (*id).bInterfaceClass) {
//        LCD_UsrLog("> Mass storage device connected.\n");
    } else if (0x03U == (*id).bInterfaceClass) {
//        LCD_UsrLog("> HID device connected.\n");
    }
}

void usbh_user_manufacturer_string(void *manufacturer_string)
{
}

void usbh_user_product_string(void *product_string)
{
}

void usbh_user_serialnum_string(void *serial_num_string)
{
}

void usbh_user_enumeration_finish(void)
{
}

void usbh_user_device_not_supported(void)
{
}

usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USBH_USER_NO_RESP;
    
    return usbh_usr_status;
}

void usbh_user_over_current_detected (void)
{
}

void usbh_user_deinit(void)
{

}

int usbh_user_app(void)
{
    return 0;
}

#endif