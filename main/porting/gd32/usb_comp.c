/**
 * @file usb_comp.c
 */

#include "usb_comp.h"
#include "usb_descriptors.h"

static uint8_t comp_init(usb_dev *udev, uint8_t config_index);
static uint8_t comp_deinit(usb_dev *udev, uint8_t config_index);
static uint8_t comp_req_proc(usb_dev *udev, usb_req *req);
static uint8_t comp_data_in(usb_dev *udev, uint8_t ep_num);
static uint8_t comp_data_out(usb_dev *udev, uint8_t ep_num);

usb_class_core usbd_comp_cb = {
    .command        = 0xFFU,//NO_CMD,
    .alter_set      = 0U,

    .init           = comp_init,
    .deinit         = comp_deinit,
    .req_proc       = comp_req_proc,
    .data_in        = comp_data_in,
    .data_out       = comp_data_out,
};

/* USB language ID Descriptor */
const usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength         = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR
     },
    .wLANGID              = ENG_LANGID
};

/* USB manufacture string */
static const usb_desc_str manufacturer_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(10U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static const usb_desc_str product_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(17U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'D', '3', '2', '-','U', 'S', 'B', '_', 'K', 'e', 'y', 'b', 'o', 'a', 'r', 'd'}
};

/* USBD serial string */
static usb_desc_str serial_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     }
};

void *const usbd_hid_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc comp_desc = {
    .strings     = usbd_hid_strings,
};

typedef struct {
    uint8_t busy;
    uint8_t protocol;
    uint8_t idle;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t data_size;
    uint8_t data_buf[HID_EP_BUF_SIZE];
} comp_itf_t;

static comp_itf_t comp_itfs[ITF_NUM_TOTAL];

bool usbd_comp_itf_ready(usb_core_driver *udev, uint32_t itf_num)
{
    if (itf_num >= ITF_NUM_TOTAL) return false;

    comp_itf_t *itf = (comp_itf_t*)(&udev->dev.class_data[itf_num]);
    return itf->busy ? true : false;
}

static comp_itf_t *find_itf(usb_core_driver *udev, uint8_t type)
{
    if (type == HID_REPORT_ID_KEYBOARD) {
        return (comp_itf_t*)udev->dev.class_data[ITF_NUM_HID_KBD];
#ifdef VIAL_ENABLE 
    } else if (type == HID_REPORT_ID_VIAL) {
        return (comp_itf_t*)udev->dev.class_data[ITF_NUM_VIAL];
#endif
    } else {
        return (comp_itf_t*)udev->dev.class_data[ITF_NUM_HID_OTHER];
    }
}

uint8_t usbd_comp_send(usb_core_driver *udev, uint8_t type, uint8_t *report, uint16_t len)
{
    comp_itf_t *itf = find_itf(udev, type);
    if (type != 0) {
        itf->data_buf[0] = type;
        memcpy(&itf->data_buf[1], report, len);
        itf->data_size = len+1;
    } else {
        memcpy(&itf->data_buf[0], report, len);
        itf->data_size = len;
    }

    itf->busy = 1;
    usbd_ep_send(udev, itf->ep_in, itf->data_buf, itf->data_size);
    return USBD_OK;
}

uint8_t comp_init(usb_dev *udev, uint8_t config_index)
{
    comp_desc.dev_desc = (uint8_t*)tud_descriptor_device_cb();
    comp_desc.config_desc = (uint8_t*)tud_descriptor_configuration_cb(0); 

    usb_hid_desc_config_set* hid_config_desc = (usb_hid_desc_config_set*)comp_desc.config_desc;

    for (int i = 0; i < ITF_NUM_TOTAL; i++) {
        memset(&comp_itfs[i], 0, sizeof(comp_itf_t));
        udev->dev.class_data[i] = &comp_itfs[i];
        if (i == ITF_NUM_HID_KBD) {
            comp_itfs[i].ep_in = 0x80 | EPNUM_HID_KBD;
            comp_itfs[i].ep_out = 0xFF;
            usbd_ep_setup (udev, &(hid_config_desc->hid_epin));
#ifdef VIAL_ENABLE 
        } else if (type == HID_REPORT_ID_VIAL) {
            comp_itfs[i].ep_in = 0x80 | EPNUM_VIAL_IN;
            comp_itfs[i].ep_out = EPNUM_VIAL_OUT;
            usbd_ep_setup(udev, &(hid_config_desc->hid_epin));
            usbd_ep_setup(udev, &(hid_config_desc->hid_epout));

            usbd_ep_recev (udev, comp_itfs[i].ep_out, comp_itfs[i].data_buf, HID_EP_BUF_SIZE);
#endif
        } else {
            comp_itfs[i].ep_in = 0x80 | EPNUM_HID_OTHER;
            comp_itfs[i].ep_out = 0xFF;
            usbd_ep_setup (udev, &(hid_config_desc->hid_epin));
        }
    }

    return USB_OK;
}

static uint8_t comp_deinit(usb_dev *udev, uint8_t config_index)
{
    for (int i = 0; i < ITF_NUM_TOTAL; i++) {
        if (i == ITF_NUM_HID_KBD) {
            usbd_ep_clear(udev, EPNUM_HID_KBD);
#ifdef VIAL_ENABLE 
        } else if (type == HID_REPORT_ID_VIAL) {
            usbd_ep_clear(udev, EPNUM_VIAL_IN);
            usbd_ep_clear(udev, EPNUM_VIAL_IN);
#endif
        } else {
            usbd_ep_clear(udev, EPNUM_HID_OTHER);
        }
    }

    return USB_OK;
}

extern uint8_t amk_led_state;
static uint8_t comp_req_proc(usb_dev *udev, usb_req *req)
{
    usb_transc *transc = &udev->dev.transc_in[0];

    if (req->wIndex >= ITF_NUM_TOTAL) return USBD_FAIL;

    comp_itf_t* itf = &comp_itfs[req->wIndex];

    switch (req->bRequest) {
    case GET_REPORT:
        // should sent last report
        break;

    case GET_IDLE:
        transc->xfer_buf = (uint8_t *)&itf->idle;
        transc->remain_len = 1U;
        break;

    case GET_PROTOCOL:
        transc->xfer_buf = (uint8_t *)&itf->protocol;
        transc->remain_len = 1U;
        break;

    case SET_REPORT:
        // process led state .etc
        if (req->wIndex == ITF_NUM_HID_KBD) {
            amk_led_state = udev->dev.transc_out[0].xfer_buf[0];
        } 
#ifdef VIAL_ENABLE
        if (req->wIndex== ITF_NUM_VIAL) {
            //amk_printf("VIAL process data: size=%d\n", bufsize);
            vial_process((uint8_t*)comp_itfs[req->wIndex].data_buf, HID_EP_BUF_SIZE);
        }
#endif
        break;

    case SET_IDLE:
        itf->idle = (uint8_t)(req->wValue >> 8U);
        break;

    case SET_PROTOCOL:
        itf->protocol = (uint8_t)(req->wValue);
        break;

    case USB_GET_DESCRIPTOR:
        if (USB_DESCTYPE_REPORT == (req->wValue >> 8U)) {
            transc->remain_len = USB_MIN(tud_hid_descriptor_report_size(req->wIndex), req->wLength);
            transc->xfer_buf = (uint8_t *)tud_hid_descriptor_report_cb(req->wIndex);

        } else if (USB_DESCTYPE_HID == (req->wValue >> 8U)) {
            transc->remain_len = USB_MIN(tud_hid_descriptor_interface_size(req->wIndex), req->wLength);
            transc->xfer_buf = (uint8_t *)tud_hid_descriptor_interface_cb(req->wIndex);
        }
        break;

    default:
        break;
    }

    return USBD_OK;
}

static uint8_t comp_data_in(usb_dev *udev, uint8_t ep_num)
{
    for (int i = 0; i < ITF_NUM_TOTAL; i++) {
        if (ep_num == comp_itfs[i].ep_in) {
            // found itf, clear the busy flag
            comp_itfs[i].busy = 0;
            break;
        }
    }
    return USB_OK;
}

static uint8_t comp_data_out(usb_dev *udev, uint8_t ep_num)
{
    for (int i = 0; i < ITF_NUM_TOTAL; i++) {
        if (ep_num == comp_itfs[i].ep_out) {
            // found itf, process data 
#ifdef VIAL_ENABLE
            if (i== ITF_NUM_VIAL) {
                //amk_printf("VIAL process data: size=%d\n", bufsize);
                vial_process((uint8_t*)comp_itfs[i].data_buf, HID_EP_BUF_SIZE);
            }
#endif
            usbd_ep_recev (udev, comp_itfs[i].ep_out, comp_itfs[i].data_buf, HID_EP_BUF_SIZE);
            break;
        }
    }

    return USB_OK;
}