/**
 * @file usbd_composite.c
 */

#include "usbd_ctlreq.h"
#include "usbd_composite.h"
#include "usb_descriptors.h"
#include "rtt.h"

//#define USBD_COMPOSITE_MAX_INTERFACE    ITF_NUM_TOTAL
// hid keyboard and other(mouse, system, consumer)
#define USBD_HID_KBD_EPIN               (0x80|EPNUM_HID_KBD)
#define USBD_HID_KBD_EPIN_SIZE          CFG_TUD_HID_EP_SIZE
#define USBD_HID_KBD_EPIN_TYPE          USBD_EP_TYPE_INTR 
#define USBD_HID_KBD_EPOUT              0
#define USBD_HID_KBD_EPOUT_SIZE         0
#define USBD_HID_KBD_EPOUT_TYPE         0

#define USBD_HID_OTHER_EPIN             (0x80|EPNUM_HID_OTHER)
#define USBD_HID_OTHER_EPIN_SIZE        CFG_TUD_HID_EP_SIZE
#define USBD_HID_OTHER_EPIN_TYPE        USBD_EP_TYPE_INTR 
#define USBD_HID_OTHER_EPOUT            0
#define USBD_HID_OTHER_EPOUT_SIZE       0
#define USBD_HID_OTHER_EPOUT_TYPE       0

// webusb 
#define USBD_VENDOR_EPIN                (0x80|EPNUM_VENDOR_IN)
#define USBD_VENDOR_EPIN_SIZE           CFG_TUD_VENDOR_EP_SIZE
#define USBD_VENDOR_EPIN_TYPE           USBD_EP_TYPE_BULK 
#define USBD_VENDOR_EPOUT               (EPNUM_VENDOR_OUT)
#define USBD_VENDOR_EPOUT_SIZE          USBD_VENDOR_EPIN_SIZE
#define USBD_VENDOR_EPOUT_TYPE          USBD_EP_TYPE_BULK

static uint8_t USBD_COMP_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMP_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMP_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t* USBD_COMP_GetFSCfgDesc(uint16_t *length);
static uint8_t USBD_COMP_EP0_TxSent(struct _USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMP_EP0_RxReady(struct _USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMP_DataIn(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMP_DataOut(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);

USBD_ClassTypeDef  USBD_COMP = {
    .Init = USBD_COMP_Init,
    .DeInit = USBD_COMP_DeInit,
    .Setup = USBD_COMP_Setup,
    .EP0_TxSent = USBD_COMP_EP0_TxSent,
    .EP0_RxReady = USBD_COMP_EP0_RxReady,
    .DataIn = USBD_COMP_DataIn,
    .DataOut = USBD_COMP_DataOut,
    .GetFSConfigDescriptor = USBD_COMP_GetFSCfgDesc,
};

typedef struct __usbd_interface usbd_interface_t;
typedef void (*interface_init_t)(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
typedef void (*interface_uninit_t)(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
// one interface only support 1 epin and 1 epout
typedef struct __usbd_interface{
    uint32_t index;         // interface index
    uint32_t epin;          // in endpoint index
    uint32_t epin_size;     // packet size of epin
    uint32_t epin_type;     // type of the epin 
    uint32_t epout;         // out endpoint index
    uint32_t epout_size;    // packet size of epout
    uint32_t epout_type;    // type of the epout 
    usbd_class_interface_t* instance;
    interface_init_t init;
    interface_uninit_t uninit;
    void* data;
} usbd_interface_t;

typedef struct {
    usbd_interface_t interfaces[ITF_NUM_TOTAL];
    uint32_t size;
} usbd_composite_t;

static usbd_interface_t* find_interface_by_epnum(uint32_t epnum);
static usbd_interface_t* find_interface_by_type(uint32_t type);
static void hid_kbd_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void hid_other_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void hid_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void webusb_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void webusb_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);

static USBD_HID_HandleTypeDef USBD_HID_DATA_KBD;
static USBD_HID_HandleTypeDef USBD_HID_DATA_OTHER;
static USBD_WEBUSB_HandleTypeDef USBD_WEBUSB_DATA;

static usbd_composite_t usbd_composite = {
    .interfaces = {
        {.index = ITF_NUM_HID_KBD,
        .epin = USBD_HID_KBD_EPIN,
        .epin_size = USBD_HID_KBD_EPIN_SIZE,
        .epin_type = USBD_HID_KBD_EPIN_TYPE,
        .epout = USBD_HID_KBD_EPOUT,
        .epout_size = USBD_HID_KBD_EPOUT_SIZE,
        .epout_type = USBD_HID_KBD_EPOUT_TYPE,
        .instance = &USBD_HID,
        .init = hid_kbd_init,
        .uninit = hid_uninit,
        .data = &USBD_HID_DATA_KBD},

        {.index = ITF_NUM_HID_OTHER,
        .epin = USBD_HID_OTHER_EPIN,
        .epin_size = USBD_HID_OTHER_EPIN_SIZE,
        .epin_type = USBD_HID_OTHER_EPIN_TYPE,
        .epout = USBD_HID_OTHER_EPOUT,
        .epout_size = USBD_HID_OTHER_EPOUT_SIZE,
        .epout_type = USBD_HID_OTHER_EPOUT_TYPE,
        .instance = &USBD_HID,
        .init = hid_other_init,
        .uninit = hid_uninit,
        .data = &USBD_HID_DATA_OTHER},

        {.index = ITF_NUM_VENDOR,
        .epin = USBD_VENDOR_EPIN,
        .epin_size = USBD_VENDOR_EPIN_SIZE,
        .epin_type = USBD_VENDOR_EPIN_TYPE,
        .epout = USBD_VENDOR_EPOUT,
        .epout_size = USBD_VENDOR_EPOUT_SIZE,
        .epout_type = USBD_VENDOR_EPOUT_TYPE,
        .instance = &USBD_WEBUSB,
        .init = webusb_init,
        .uninit = webusb_uninit,
        .data = &USBD_WEBUSB_DATA},
    },
    .size = ITF_NUM_TOTAL 
};

static uint8_t  USBD_COMP_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;
    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        interface->init(pdev, interface);
    }

    pdev->pClassData = &usbd_composite;
    return USBD_OK;
}

static uint8_t  USBD_COMP_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        interface->uninit(pdev, interface);
    }

    pdev->pClassData = NULL;
    return USBD_OK;
}

uint8_t USBD_COMP_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_StatusTypeDef ret = USBD_FAIL;
    //rtt_printf("COMP:bmRequst=%d, request=%d, index=%d, value=%d, length=%d\n", req->bmRequest, req->bRequest, req->wIndex, req->wValue, req->wLength);
    if ((req->bmRequest&USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
        usbd_interface_t* interface = &usbd_composite.interfaces[ITF_NUM_VENDOR];
        ret = interface->instance->Setup(pdev, req, interface->data);
    } else {
        uint8_t receipt = req->bmRequest&USB_REQ_RECIPIENT_MASK;
        if (receipt == USB_REQ_RECIPIENT_INTERFACE)
        {
            if (req->wIndex < usbd_composite.size) {
                usbd_interface_t* interface = &usbd_composite.interfaces[req->wIndex];
                ret = interface->instance->Setup(pdev, req, interface->data);
            } else {
                rtt_printf("Setup requests to interface=%d not supported\n", receipt);
                ret = USBD_FAIL;
            }
        } else {
            rtt_printf("Setup request to receipt=%d not supported\n", receipt);
            ret = USBD_FAIL;
        }
    } 
    return ret;
}

uint8_t* USBD_COMP_GetFSCfgDesc(uint16_t *length)
{
    *length = tud_descriptor_configuration_size();
    return tud_descriptor_configuration_cb();
}

uint8_t USBD_COMP_EP0_TxSent(struct _USBD_HandleTypeDef *pdev)
{
    return USBD_OK;
}

uint8_t USBD_COMP_EP0_RxReady(struct _USBD_HandleTypeDef *pdev)
{
    return USBD_OK;
}

uint8_t USBD_COMP_DataIn(struct _USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    usbd_interface_t* interface = find_interface_by_epnum(epnum);
    rtt_printf("COMP DataIn: epnum=%d\n", epnum);

    if (interface) {
        return interface->instance->DataIn(pdev, epnum, interface->data);
    }

    return USBD_FAIL;
}

uint8_t USBD_COMP_DataOut(struct _USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    usbd_interface_t* interface = find_interface_by_epnum(epnum);
    rtt_printf("COMP DataOut: epnum=%d\n", epnum);
    
    if (interface && interface->instance->DataOut) {
        return interface->instance->DataOut(pdev, epnum, interface->data);
    }

    return USBD_FAIL;
}

uint8_t USBD_COMP_Send(USBD_HandleTypeDef *pdev, uint8_t type, uint8_t *report, uint16_t len)
{
    if (pdev->dev_state != USBD_STATE_CONFIGURED) {
        return USBD_FAIL;
    }
    usbd_interface_t* interface = find_interface_by_type(type);
    if (interface && interface->instance->Write) {
        return interface->instance->Write(pdev, interface->epin, report, len, interface->data);
    }

    return USBD_FAIL;
}

usbd_interface_t* find_interface_by_epnum(uint32_t epnum)
{
    if (epnum == 0) return NULL;

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        if ((epnum==(interface->epin&EP_ADDR_MSK)) || (epnum==interface->epout)) {
            return interface;
        }
    }

    return NULL;
}

usbd_interface_t* find_interface_by_type(uint32_t type)
{
    if (type == HID_REPORT_ID_KEYBOARD) {
        return &usbd_composite.interfaces[ITF_NUM_HID_KBD];
    } else {
        if (type == HID_REPORT_ID_WEBUSB) {
            return &usbd_composite.interfaces[ITF_NUM_VENDOR];
        } else {
            return &usbd_composite.interfaces[ITF_NUM_HID_OTHER];
        }
    }
}

void hid_kbd_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    pdev->ep_in[interface->epin & EP_ADDR_MSK].is_used = 1U;
    ((USBD_HID_HandleTypeDef *)interface->data)->State = HID_IDLE;
    ((USBD_HID_HandleTypeDef *)interface->data)->IsKeyboard = 1;
}

void hid_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_LL_CloseEP(pdev, interface->epin);
    pdev->ep_in[interface->epin & EP_ADDR_MSK].is_used = 0U;
}

void hid_other_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    pdev->ep_in[interface->epin & EP_ADDR_MSK].is_used = 1U;
    ((USBD_HID_HandleTypeDef *)interface->data)->State = HID_IDLE;
    ((USBD_HID_HandleTypeDef *)interface->data)->IsKeyboard = 0;
}

void webusb_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_StatusTypeDef ret = USBD_OK;
    ret = USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    rtt_printf("WEBUSB init, Open epin=%d, status=%d\n", interface->epin, ret);
    pdev->ep_in[interface->epin & EP_ADDR_MSK].is_used = 1U;
    ret = USBD_LL_OpenEP(pdev, interface->epout, interface->epout_type, interface->epout_size);
    rtt_printf("WEBUSB init, Open epout=%d, status=%d\n", interface->epout, ret);
    pdev->ep_out[interface->epout].is_used = 1U;

    USBD_WEBUSB_HandleTypeDef* ph = (USBD_WEBUSB_HandleTypeDef *)interface->data;
    ret = USBD_LL_PrepareReceive(pdev, interface->epout, ph->recv_buffer, WEBUSB_PACKET_SIZE);
    rtt_printf("WEBUSB init: prepare receive: epnum=%d, status=%d\n", interface->epout, ret);
    ph->state = WEBUSB_IDLE;
}

void webusb_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_LL_CloseEP(pdev, interface->epin);
    pdev->ep_in[interface->epin & EP_ADDR_MSK].is_used = 0U;
    USBD_LL_CloseEP(pdev, interface->epout);
    pdev->ep_out[interface->epout].is_used = 0U;
}