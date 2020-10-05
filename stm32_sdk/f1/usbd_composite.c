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
#define USBD_VENDOR_EPIN                (0x80|EPNUM_VENDOR)
#define USBD_VENDOR_EPIN_SIZE           CFG_TUD_VENDOR_EP_SIZE
#define USBD_VENDOR_EPIN_TYPE           USBD_EP_TYPE_BULK 
#define USBD_VENDOR_EPOUT               EPNUM_VENDOR
#define USBD_VENDOR_EPOUT_SIZE          USBD_VENDOR_EPIN_SIZE
#define USBD_VENDOR_EPOUT_TYPE          USBD_EP_TYPE_BULK

static uint8_t  USBD_COMP_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_COMP_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_COMP_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  *USBD_COMP_GetFSCfgDesc(uint16_t *length);
static uint8_t USBD_COMP_EP0_TxSent(struct _USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMP_EP0_RxReady(struct _USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMP_DataIn(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMP_DataOut(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);

USBD_ClassTypeDef  USBD_COMP =
{
  USBD_COMP_Init,
  USBD_COMP_DeInit,
  USBD_COMP_Setup,
  USBD_COMP_EP0_TxSent,
  USBD_COMP_EP0_RxReady,
  USBD_COMP_DataIn,
  USBD_COMP_DataOut,
  NULL,
  NULL,
  NULL,
  NULL,
  USBD_COMP_GetFSCfgDesc,
  NULL,
  NULL,
};


// one interface only support 1 epin and 1 epout
typedef struct {
    uint32_t index;         // interface index
    uint32_t epin;          // in endpoint index
    uint32_t epin_size;     // packet size of epin
    uint32_t epin_type;     // type of the epin 
    uint32_t epout;         // out endpoint index
    uint32_t epout_size;    // packet size of epout
    uint32_t epout_type;    // type of the epout 
    usbd_class_interface_t* instance;
    void* data;
} usbd_interface_t;

typedef struct {
    usbd_interface_t interfaces[USBD_MAX_NUM_INTERFACES];
    uint32_t size;
} usbd_composite_t;

static usbd_interface_t* find_interface_by_epnum(uint32_t epnum);

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
        .data = &USBD_HID_DATA_KBD},

        {.index = ITF_NUM_HID_OTHER,
        .epin = USBD_HID_OTHER_EPIN,
        .epin_size = USBD_HID_OTHER_EPIN_SIZE,
        .epin_type = USBD_HID_OTHER_EPIN_TYPE,
        .epout = USBD_HID_OTHER_EPOUT,
        .epout_size = USBD_HID_OTHER_EPOUT_SIZE,
        .epout_type = USBD_HID_OTHER_EPOUT_TYPE,
        .instance = &USBD_HID,
        .data = &USBD_HID_DATA_OTHER},

        {.index = ITF_NUM_VENDOR,
        .epin = USBD_VENDOR_EPIN,
        .epin_size = USBD_VENDOR_EPIN_SIZE,
        .epin_type = USBD_VENDOR_EPIN_TYPE,
        .epout = USBD_VENDOR_EPOUT,
        .epout_size = USBD_VENDOR_EPOUT_SIZE,
        .epout_type = USBD_VENDOR_EPOUT_TYPE,
        .instance = &USBD_WEBUSB,
        .data = &USBD_WEBUSB_DATA},
    },
    .size = USBD_MAX_NUM_INTERFACES
};

static uint8_t  USBD_COMP_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;
    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
        pdev->ep_in[interface->epin & 0xFU].is_used = 1U;
        if (interface->epout) {
            USBD_LL_OpenEP(pdev, interface->epout, interface->epout_type, interface->epout_size);
            pdev->ep_out[interface->epout].is_used = 1U;
        }
    }

    pdev->pClassData = &usbd_composite;

    ((USBD_HID_HandleTypeDef *)usbd_composite.interfaces[0].data)->state = HID_IDLE;
    ((USBD_HID_HandleTypeDef *)usbd_composite.interfaces[0].data)->IsKeyboard = 1;
    ((USBD_HID_HandleTypeDef *)usbd_composite.interfaces[1].data)->state = HID_IDLE;
    ((USBD_HID_HandleTypeDef *)usbd_composite.interfaces[1].data)->IsKeyboard = 0;
    ((USBD_WEBUSB_HandleTypeDef *)usbd_composite.interfaces[2].data)->state = WEBUSB_IDLE;

    return USBD_OK;
}

static uint8_t  USBD_COMP_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        USBD_LL_CloseEP(pdev, interface->epin);
        pdev->ep_in[interface->epin & 0xFU].is_used = 0U;
        if (interface->epout) {
            USBD_LL_CloseEP(pdev, interface->epout);
            pdev->ep_out[interface->epout].is_used = 0U;
        }
    }

    pdev->pClassData = NULL;
    return USBD_OK;
}

uint8_t USBD_COMP_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_StatusTypeDef ret = USBD_FAIL;
    rtt_printf("bmRequst=%d, request=%d, index=%d\n", req->bmRequest, req->bRequest, req->wIndex);
    if ((req->bmRequest&USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
        usbd_interface_t* interface = &usbd_composite.interfaces[2];
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
    
    if (interface) {
        return interface->instance->DataIn(pdev, epnum, interface->data);
    }

    return USBD_FAIL;
}

uint8_t USBD_COMP_DataOut(struct _USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    usbd_interface_t* interface = find_interface_by_epnum(epnum);
    
    if (interface) {
        return interface->instance->DataOut(pdev, epnum, interface->data);
    }

    return USBD_FAIL;
}

usbd_interface_t* find_interface_by_epnum(uint32_t epnum)
{
    if (epnum == 0) return NULL;

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        if ((epnum==interface->epin) || (epnum==interface->epout)) {
            return interface;
        }
    }

    return NULL;
}