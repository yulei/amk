/**
 * usb_tinyusb.c
 */

#include "usb_interface.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "amk_printf.h"

#define QUEUE_ITEM_SIZE 16                          // maximum size of the queue item
typedef struct {
    uint32_t    type;                               // type of the item
    uint32_t    size;
    uint8_t     data[QUEUE_ITEM_SIZE];
} report_item_t;

#define QUEUE_SIZE      16 
typedef struct {
    report_item_t   items[QUEUE_SIZE];
    uint32_t        head;
    uint32_t        tail;
} report_queue_t;

static report_queue_t report_queue;

static bool report_queue_empty(report_queue_t* queue)
{
    return queue->head == queue->tail;
}

static bool report_queue_full(report_queue_t* queue)
{
    return ((queue->tail + 1) % QUEUE_SIZE) == queue->head;
}

static void report_queue_init(report_queue_t* queue)
{
    memset(&queue->items[0], 0, sizeof(queue->items));
    queue->head = queue->tail = 0;
}

static bool report_queue_put(report_queue_t* queue, report_item_t* item)
{
    if (report_queue_full(queue)) return false;

    queue->items[queue->tail] = *item;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    return true;
}

static bool report_queue_get(report_queue_t* queue, report_item_t* item)
{
    if (report_queue_empty(queue)) return false;

    *item = queue->items[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    return true;
}

void usb_init(void)
{
    tusb_init();
    report_queue_init(&report_queue);
}

void usb_task(void)
{
    tud_task();

    if( !report_queue_empty(&report_queue)) {
        amk_printf("dequeue buffered report\n");
        report_item_t item;
        report_queue_get(&report_queue, &item);
        usb_send_report(item.type, item.data, item.size);
    }
}

bool usb_ready(void)
{
    return tud_hid_ready();
}

bool usb_suspended(void)
{
    return tud_suspended();
}

void usb_remote_wakeup(void)
{
#if defined(OPT_MCU_STM32F1)
    tud_remote_wakeup();
#elif defined(OPT_MCU_STM32F4)
#define USB_DEVICE     ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
    USB_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
    HAL_Delay(5);
    USB_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
#else
#error "Unsupport MCU on remote wakeup"
#endif
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    static report_item_t item;
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        if (!tud_hid_n_ready(ITF_NUM_HID_KBD)) {
            amk_printf("Keyboard interface busy or not ready, enqueue report\n");
            memcpy(item.data, data, size);
            item.type = report_type;
            item.size = size;
            report_queue_put(&report_queue, &item);
        } else {
            tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size);
        }
        break;
    case HID_REPORT_ID_MOUSE:
        if (!tud_hid_n_ready(ITF_NUM_HID_OTHER)) {
            amk_printf("Mouse interface busy or not ready, enqueue report\n");
            memcpy(item.data, data, size);
            item.type = report_type;
            item.size = size;
            report_queue_put(&report_queue, &item);
        } else {
            tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, data, (uint8_t)size);
        }
        break;
    case HID_REPORT_ID_SYSTEM:
        if (!tud_hid_n_ready(ITF_NUM_HID_OTHER)) {
            amk_printf("System interface busy or not ready, enqueue report\n");
            memcpy(item.data, data, size);
            item.type = report_type;
            item.size = size;
            report_queue_put(&report_queue, &item);
        } else {
            tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, data, (uint8_t)size);
        }
        break;
    case HID_REPORT_ID_CONSUMER:
        if (!tud_hid_n_ready(ITF_NUM_HID_OTHER)) {
            amk_printf("Consumer interface busy or not ready, enqueue report\n");
            memcpy(item.data, data, size);
            item.type = report_type;
            item.size = size;
            report_queue_put(&report_queue, &item);
        } else {
            tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_CONSUMER, data, (uint8_t)size);
        }
      break;
    }
}

//=============================
// tusb callback
//=============================
// Invoked when device is mounted
void tud_mount_cb(void) { }

// Invoked when device is unmounted
void tud_umount_cb(void) { }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
extern uint8_t amk_led_state;
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    //amk_printf("Set Report:\n")
    (void) report_id;
    if (itf == ITF_NUM_HID_KBD && report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize) {
            amk_led_state = buffer[0];
            amk_printf("Set Report Data: size=%d, state=%x\n", bufsize, buffer[0]);
        }
    }
}