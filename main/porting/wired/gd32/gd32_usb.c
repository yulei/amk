/**
 * @file gd32_usb.c
 * 
 */

#include "amk_usb.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "drv_usb_hw.h"
#include "drv_usb_core.h"
#include "amk_printf.h"

#include "report.h"
#include "wait.h"

#ifdef USB_HOST_ENABLE
#include "usbh_hid_core.h"
#include "usbh_hid_mouse.h"

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
    usbh_class_register(&usb_host, &usbh_hid);
    usbh_init(&usb_host, &user_cb);
    usb_intr_config();
#endif
}

void amk_usb_task(void)
{
#ifdef USB_HOST_ENABLE
    usbh_core_task(&usb_host);
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
    amk_printf("usbh_user_init:\n");
}

void usbh_user_device_connected(void)
{
    amk_printf("usbh_user_device_connected:\n");
}

void usbh_user_unrecovered_error (void)
{
    amk_printf("usbh_user_unrecovered_erro:\n");
}

void usbh_user_device_disconnected (void)
{
    amk_printf("usbh_user_device_disconnected:\n");
}

void usbh_user_device_reset(void)
{
    amk_printf("usbh_user_device_reset:\n");
}

void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if (PORT_SPEED_HIGH == device_speed) {
    } else if(PORT_SPEED_FULL == device_speed) {
    } else if(PORT_SPEED_LOW == device_speed) {
    } else {
    }
    amk_printf("usbh_user_device_speed_detected:\n");
}

void usbh_user_device_desc_available(void *device_desc)
{
    //usb_desc_dev *pDevStr = device_desc;
    amk_printf("usbh_user_device_desc_available:\n");
}

void usbh_user_device_address_assigned(void)
{
    amk_printf("usbh_user_device_address_assigned:\n");
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
    amk_printf("usbh_user_configuration_descavailable:\n");
}

void usbh_user_manufacturer_string(void *manufacturer_string)
{
    amk_printf("usbh_user_manufacturer_string: %s\n", (char*)manufacturer_string);
}

void usbh_user_product_string(void *product_string)
{
    amk_printf("usbh_user_product_string:\n");
}

void usbh_user_serialnum_string(void *serial_num_string)
{
    amk_printf("usbh_user_serialnum_string:\n");
}

void usbh_user_enumeration_finish(void)
{
    amk_printf("usbh_user_enumeration_finish:\n");
}

void usbh_user_device_not_supported(void)
{
    amk_printf("usbh_user_device_not_supported:\n");
}

usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USBH_USER_RESP_OK;
    
    //amk_printf("usbh_user_userinput:\n");
    return usbh_usr_status;
}

void usbh_user_over_current_detected (void)
{
    amk_printf("usbh_user_over_current_detected:\n");
}

void usbh_user_deinit(void)
{
    amk_printf("usbh_user_over_deinit:\n");
}

int usbh_user_app(void)
{
    amk_printf("usbh_user_app:\n");
    return 0;
}

void USR_KEYBRD_Init (void)
{
    amk_printf("USR_KEYBRD_Init:\n");
}

void USR_KEYBRD_ProcessData (uint8_t data)
{
    amk_printf("USR_KEYBRD_ProcessData: %x\n", data);
}

void USR_MOUSE_Init (void)
{
    amk_printf("USR_MOUSE_Init:\n");
}

void USR_MOUSE_ProcessData (hid_mouse_info *data)
{
    amk_printf("USR_MOUSE_ProcessData:\n");
}
#endif