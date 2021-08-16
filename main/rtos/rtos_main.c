/**
 * @file rtos_main.c
 */

#include <stdint.h>
#include "tx_api.h"
#include "ux_api.h"
#include "ux_device_class_hid.h"
#include "usbx_desc.h"
#include "board.h"

#define MAIN_THREAD_PRIO            10
#define MAIN_PREEMPTION_THRESHOLD   (MAIN_THREAD_PRIO)
#define MAIN_STACK_SIZE             4096
static CHAR main_stack[MAIN_STACK_SIZE];

#define USBX_MEMORY_SIZE        (32*1024)
static CHAR usbx_memory[USBX_MEMORY_SIZE];

static UCHAR keyboad_class_name[] = "HID keyboard";
static UCHAR other_class_name[] = "HID other";
static VOID hid_keyboard_activate(VOID *);
static VOID hid_keyboard_deactivate(VOID *);
static VOID hid_other_activate(VOID *);
static VOID hid_other_deactivate(VOID *);

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
    hid_parameter.ux_slave_class_hid_instance_activate = hid_keyboard_activate;
    hid_parameter.ux_slave_class_hid_instance_deactivate = hid_keyboard_deactivate; 
    hid_parameter.ux_device_class_hid_parameter_report_address = usbx_desc_hid_keyboard();
    hid_parameter.ux_device_class_hid_parameter_report_id = UX_FALSE;
    hid_parameter.ux_device_class_hid_parameter_report_length = usbx_desc_hid_keyboard_size();
    hid_parameter.ux_device_class_hid_parameter_callback = usbx_device_hid_report_callback;
    hid_parameter.ux_device_class_hid_parameter_get_callback = UX_NULL;
    status = ux_device_stack_class_register(keyboad_class_name, ux_device_class_hid_entry, 1, ITF_NUM_HID_KBD, (VOID*)&hid_parameter);

    // register other
    hid_parameter.ux_slave_class_hid_instance_activate = hid_other_activate;
    hid_parameter.ux_slave_class_hid_instance_deactivate = hid_other_deactivate; 
    hid_parameter.ux_device_class_hid_parameter_report_address = usbx_desc_hid_other();
    hid_parameter.ux_device_class_hid_parameter_report_id = UX_TRUE;
    hid_parameter.ux_device_class_hid_parameter_report_length = usbx_desc_hid_other_size();
    hid_parameter.ux_device_class_hid_parameter_callback = UX_NULL;
    hid_parameter.ux_device_class_hid_parameter_get_callback = UX_NULL;
    status = ux_device_stack_class_register(other_class_name, ux_device_class_hid_entry, 1, ITF_NUM_HID_OTHER, (VOID*)&hid_parameter);


#ifdef MSC_ENABLE
    extern void usbx_msc_init(void);
    usbx_msc_init();
#endif
    tx_thread_create(&main_thread, "main thread", main_thread_entry, 0, main_stack, MAIN_STACK_SIZE, 
                    MAIN_THREAD_PRIO, MAIN_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);
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
       tx_thread_sleep(5);
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

extern UX_SLAVE_CLASS_HID *keyboard_class;
static VOID hid_keyboard_activate(VOID *inst)
{
    keyboard_class = (UX_SLAVE_CLASS_HID*)inst;
}

static VOID hid_keyboard_deactivate(VOID *inst)
{
    keyboard_class = UX_NULL;
}

extern UX_SLAVE_CLASS_HID *other_class;
static VOID hid_other_activate(VOID *inst)
{
    other_class = (UX_SLAVE_CLASS_HID*)inst;
}

static VOID hid_other_deactivate(VOID *inst)
{
    other_class = UX_NULL;
}
static void error_handler(void)
{
    __asm__("BKPT");
}
