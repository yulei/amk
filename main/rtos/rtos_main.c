/**
 * @file rtos_main.c
 */

#include <stdint.h>
#include "tx_api.h"
#include "ux_api.h"
#include "ux_device_class_hid.h"
#include "usbx_desc.h"
#include "board.h"

#define MAIN_STACK_SIZE         4096
static CHAR main_stack[MAIN_STACK_SIZE];

#define USBX_MEMORY_SIZE        (32*1024)
static CHAR usbx_memory[USBX_MEMORY_SIZE];

static UCHAR keyboad_class_name[] = "HID keyboard";
static UCHAR other_class_name[] = "HID other";

TX_THREAD main_thread;
void main_thread_entry(ULONG thread_input);

static void error_handler(void);
static UINT usbx_device_change_callback(ULONG state);

static UINT usbx_device_hid_report_callback(struct UX_SLAVE_CLASS_HID_STRUCT *hid, UX_SLAVE_CLASS_HID_EVENT *);

UX_SLAVE_CLASS_HID_PARAMETER hid_parameter;

void tx_application_define(void *first_unused_memory)
{
    UINT status = ux_system_initialize(usbx_memory, USBX_MEMORY_SIZE, UX_NULL, 0);

    if (status != UX_SUCCESS) {
        error_handler();
    }

    status =  ux_device_stack_initialize(UX_NULL, 0, 
                                        usbx_desc_device_fullspeed(), usbx_desc_device_fullspeed_size(), 
                                        usbx_desc_string(), usbx_desc_string_size(),
                                        usbx_desc_language_id(), usbx_desc_language_id_size(), usbx_device_change_callback);

    if (status != UX_SUCCESS) {
        error_handler();
    }

    // register keyboard 
    hid_parameter.ux_slave_class_hid_instance_activate = UX_NULL;
    hid_parameter.ux_slave_class_hid_instance_deactivate = UX_NULL; 
    hid_parameter.ux_device_class_hid_parameter_report_address = usbx_desc_hid_keyboard();
    hid_parameter.ux_device_class_hid_parameter_report_id = UX_FALSE;
    hid_parameter.ux_device_class_hid_parameter_report_length = usbx_desc_hid_keyboard_size();
    hid_parameter.ux_device_class_hid_parameter_callback = usbx_device_hid_report_callback;
    hid_parameter.ux_device_class_hid_parameter_get_callback = UX_NULL;
    status = ux_device_stack_class_register(keyboad_class_name, ux_device_class_hid_entry, 1, ITF_NUM_HID_KBD, (VOID*)&hid_parameter);

    // register other
    hid_parameter.ux_slave_class_hid_instance_activate = UX_NULL;
    hid_parameter.ux_slave_class_hid_instance_deactivate = UX_NULL; 
    hid_parameter.ux_device_class_hid_parameter_report_address = usbx_desc_hid_other();
    hid_parameter.ux_device_class_hid_parameter_report_id = UX_TRUE;
    hid_parameter.ux_device_class_hid_parameter_report_length = usbx_desc_hid_other_size();
    hid_parameter.ux_device_class_hid_parameter_callback = UX_NULL;
    hid_parameter.ux_device_class_hid_parameter_get_callback = UX_NULL;
    status = ux_device_stack_class_register(other_class_name, ux_device_class_hid_entry, 1, ITF_NUM_HID_OTHER, (VOID*)&hid_parameter);


    tx_thread_create(&main_thread, "main thread", main_thread_entry, 0, main_stack, MAIN_STACK_SIZE, 
                    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    // start threadx
    tx_kernel_enter();

    // never reach here
    return 0;
}

void main_thread_entry(ULONG thread_input)
{
    extern void usb_init(void);
    usb_init();
    while (1) {
       board_task();
       tx_thread_sleep(0.005 * TX_TIMER_TICKS_PER_SECOND);
    }
}

static UINT usbx_device_change_callback(ULONG state)
{
    return UX_SUCCESS;
}

extern uint8_t amk_led_state;
static UINT usbx_device_hid_report_callback(struct UX_SLAVE_CLASS_HID_STRUCT *hid, UX_SLAVE_CLASS_HID_EVENT *evt)
{
    amk_led_state = evt->ux_device_class_hid_event_buffer[0];
    return UX_SUCCESS;
}

static void error_handler(void)
{
    __asm__("BKPT");
}
