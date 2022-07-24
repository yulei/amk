/**
 * @file usbd_composite.c
 */

#include "usbd_ioreq.h"
#include "usbd_ctlreq.h"
#include "usbd_composite.h"
#include "usb_desc_def.h"
#include "udd_desc.h"
#include "usb_led.h"
#include "amk_printf.h"

#ifdef VIAL_ENABLE
#include "vial_porting.h"
#endif

bool usb_led_event = false;

#define HID_REQ_SET_PROTOCOL                       0x0BU
#define HID_REQ_GET_PROTOCOL                       0x03U

#define HID_REQ_SET_IDLE                           0x0AU
#define HID_REQ_GET_IDLE                           0x02U

#define HID_REQ_SET_REPORT                         0x09U
#define HID_REQ_GET_REPORT                         0x01U

typedef enum {
    ITF_IDLE = 0,
    ITF_BUSY,
} ITF_StateTypeDef;

typedef uint8_t (*setup_t)(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user); 
typedef uint8_t (*datain_t)(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
typedef uint8_t (*dataout_t)(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
typedef uint8_t (*write_t)(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user);

typedef struct {
    setup_t     setup;
    datain_t    datain;
    dataout_t   dataout;
    write_t     write;
} usbd_class_interface_t;

//#define USBD_COMPOSITE_MAX_INTERFACE    ITF_NUM_TOTAL
// hid keyboard and other(mouse, system, consumer)
#define USBD_HID_KBD_EPIN               (0x80|EPNUM_HID_KBD)
#define USBD_HID_KBD_EPIN_SIZE          UDD_HID_EP_SIZE
#define USBD_HID_KBD_EPIN_TYPE          USBD_EP_TYPE_INTR 
#define USBD_HID_KBD_EPOUT              0
#define USBD_HID_KBD_EPOUT_SIZE         0
#define USBD_HID_KBD_EPOUT_TYPE         0

#define USBD_HID_OTHER_EPIN             (0x80|EPNUM_HID_OTHER)
#define USBD_HID_OTHER_EPIN_SIZE        UDD_HID_EP_SIZE
#define USBD_HID_OTHER_EPIN_TYPE        USBD_EP_TYPE_INTR 
#define USBD_HID_OTHER_EPOUT            0
#define USBD_HID_OTHER_EPOUT_SIZE       0
#define USBD_HID_OTHER_EPOUT_TYPE       0

#define USBD_HID_VIAL_EPIN             (0x80|EPNUM_VIAL_IN)
#define USBD_HID_VIAL_EPIN_SIZE        UDD_HID_EP_SIZE
#define USBD_HID_VIAL_EPIN_TYPE        USBD_EP_TYPE_INTR 
#define USBD_HID_VIAL_EPOUT            EPNUM_VIAL_OUT
#define USBD_HID_VIAL_EPOUT_SIZE       UDD_HID_EP_SIZE
#define USBD_HID_VIAL_EPOUT_TYPE       EPNUM_VIAL_IN

enum {
    HID_HANDLE_TYPE_KEYBOARD,
    HID_HANDLE_TYPE_OTHER,
    HID_HANDLE_TYPE_VIAL,
};

typedef struct {
    ITF_StateTypeDef    state;
    uint32_t            protocol;
    uint32_t            idle;
    uint32_t            alt;
    uint32_t            type;
#ifdef VIAL_ENABLE
    uint8_t             report[USBD_HID_VIAL_EPOUT_SIZE];
#endif
} HID_HandleTypeDef;

static uint8_t hid_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user);
static uint8_t hid_datain(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t hid_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t hid_write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user);

usbd_class_interface_t USBD_HID = {
    hid_setup,
    hid_datain,
    hid_dataout,
    hid_write,
};

#ifdef MSC_ENABLE

#include  "usbd_msc_bot.h"
#include  "usbd_msc_scsi.h"

/* MSC Class Config */
#define MSC_MEDIA_PACKET            UDD_MSC_BUFFER_SIZE
#define MSC_MAX_FS_PACKET           UDD_MSC_EP_SIZE

#define BOT_GET_MAX_LUN             0xFE
#define BOT_RESET                   0xFF

#define USBD_MSC_EPIN               (0x80|EPNUM_MSC_IN)
#define USBD_MSC_EPIN_SIZE          MSC_MAX_FS_PACKET
#define USBD_MSC_EPIN_TYPE          USBD_EP_TYPE_BULK 
#define USBD_MSC_EPOUT              (EPNUM_MSC_OUT)
#define USBD_MSC_EPOUT_SIZE         USBD_MSC_EPIN_SIZE
#define USBD_MSC_EPOUT_TYPE         USBD_EP_TYPE_BULK

#define MSC_EPOUT_ADDR              USBD_MSC_EPOUT
#define MSC_EPIN_ADDR               USBD_MSC_EPIN

typedef struct
{
  uint32_t                 max_lun;
  uint32_t                 interface;
  uint8_t                  bot_state;
  uint8_t                  bot_status;
  uint32_t                 bot_data_length;
  uint8_t                  bot_data[MSC_MEDIA_PACKET];
  USBD_MSC_BOT_CBWTypeDef  cbw;
  USBD_MSC_BOT_CSWTypeDef  csw;

  USBD_SCSI_SenseTypeDef   scsi_sense [SENSE_LIST_DEEPTH];
  uint8_t                  scsi_sense_head;
  uint8_t                  scsi_sense_tail;
  uint8_t                  scsi_medium_state;

  uint16_t                 scsi_blk_size;
  uint32_t                 scsi_blk_nbr;

  uint32_t                 scsi_blk_addr;
  uint32_t                 scsi_blk_len;
} USBD_MSC_BOT_HandleTypeDef;

static uint8_t  msc_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user); 
static uint8_t  msc_datain(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t  msc_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t  msc_write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user);

usbd_class_interface_t USBD_MSC = {
    msc_setup,
    msc_datain,
    msc_dataout,
    msc_write,
};

#endif

static uint8_t comp_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t comp_deinit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t comp_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t *comp_get_fscfgdesc(uint16_t *length);
static uint8_t comp_ep0_tx_sent(struct _USBD_HandleTypeDef *pdev);
static uint8_t comp_ep0_rx_ready(struct _USBD_HandleTypeDef *pdev);
static uint8_t comp_datain(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t comp_dataout(struct _USBD_HandleTypeDef *pdev, uint8_t epnum);

USBD_ClassTypeDef  USBD_COMP = {
    .Init = comp_init,
    .DeInit = comp_deinit,
    .Setup = comp_setup,
    .EP0_TxSent = comp_ep0_tx_sent,
    .EP0_RxReady = comp_ep0_rx_ready,
    .DataIn = comp_datain,
    .DataOut = comp_dataout,
    .GetFSConfigDescriptor = comp_get_fscfgdesc,
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
    usbd_interface_t interfaces[USBD_MAX_NUM_INTERFACES];
    uint32_t size;
} usbd_composite_t;

static usbd_interface_t* find_interface_by_epnum(uint32_t epnum);
static usbd_interface_t* find_interface_by_type(uint32_t type);
static void hid_kbd_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void hid_other_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void hid_vial_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void hid_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);

static HID_HandleTypeDef USBD_HID_DATA_KBD;
static HID_HandleTypeDef USBD_HID_DATA_OTHER;
static HID_HandleTypeDef USBD_HID_DATA_VIAL;

#ifdef MSC_ENABLE
static void msc_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static void msc_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface);
static USBD_MSC_BOT_HandleTypeDef USBD_MSC_DATA;
#endif

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
#ifdef VIAL_ENABLE
        {.index = ITF_NUM_VIAL,
        .epin = USBD_HID_VIAL_EPIN,
        .epin_size = USBD_HID_VIAL_EPIN_SIZE,
        .epin_type = USBD_HID_VIAL_EPIN_TYPE,
        .epout = USBD_HID_VIAL_EPOUT,
        .epout_size = USBD_HID_VIAL_EPOUT_SIZE,
        .epout_type = USBD_HID_VIAL_EPOUT_TYPE,
        .instance = &USBD_HID,
        .init = hid_vial_init,
        .uninit = hid_uninit,
        .data = &USBD_HID_DATA_VIAL},
#endif
#ifdef MSC_ENABLE
        {.index = ITF_NUM_MSC,
        .epin = USBD_MSC_EPIN,
        .epin_size = USBD_MSC_EPIN_SIZE,
        .epin_type = USBD_MSC_EPIN_TYPE,
        .epout = USBD_MSC_EPOUT,
        .epout_size = USBD_MSC_EPOUT_SIZE,
        .epout_type = USBD_MSC_EPOUT_TYPE,
        .instance = &USBD_MSC,
        .init = msc_init,
        .uninit = msc_uninit,
        .data = &USBD_MSC_DATA},
#endif
    },
    .size = USBD_MAX_NUM_INTERFACES
};

// ================================================================================
// composite device interface
// ================================================================================
static uint8_t  comp_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;
    amk_printf("COMP Init: cfgidx=%d\n", cfgidx);
    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        interface->init(pdev, interface);
    }

    pdev->pClassData = &usbd_composite;
    return USBD_OK;
}

static uint8_t  comp_deinit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    (void)cfgidx;

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        interface->uninit(pdev, interface);
    }

    pdev->pClassData = NULL;
    return USBD_OK;
}

static uint8_t comp_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_StatusTypeDef ret = USBD_FAIL;
    amk_printf("bmRequst=%d, request=%d, index=%d, value=%d\n", req->bmRequest, req->bRequest, req->wIndex, req->wValue);
    if ((req->bmRequest&USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
        usbd_interface_t* interface = &usbd_composite.interfaces[2];
        ret = interface->instance->setup(pdev, req, interface->data);
    } else {
        uint8_t receipt = req->bmRequest&USB_REQ_RECIPIENT_MASK;
        if (receipt == USB_REQ_RECIPIENT_INTERFACE)
        {
            if (req->wIndex < usbd_composite.size) {
                usbd_interface_t* interface = &usbd_composite.interfaces[req->wIndex];
                ret = interface->instance->setup(pdev, req, interface->data);
            } else {
                amk_printf("Setup requests to interface=%d not supported\n", receipt);
                ret = USBD_FAIL;
            }
        } else {
            amk_printf("Setup request to receipt=%d not supported\n", receipt);
            ret = USBD_FAIL;
        }
    } 
    return ret;
}

static uint8_t* comp_get_fscfgdesc(uint16_t *length)
{
    *length = udd_descriptor_configuration_size(0);
    return (uint8_t*)udd_descriptor_configuration(0);
}

static uint8_t comp_ep0_tx_sent(struct _USBD_HandleTypeDef *pdev)
{
    return USBD_OK;
}

static uint8_t comp_ep0_rx_ready(struct _USBD_HandleTypeDef *pdev)
{
    amk_printf("ep0 rx ready: led =0x%x\n", amk_led_state);
    usb_led_event = true;
    return USBD_OK;
}

static uint8_t comp_datain(struct _USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    usbd_interface_t* interface = find_interface_by_epnum(epnum);
    amk_printf("COMP DataIn: epnum=%d\n", epnum);

    if (interface && interface->instance->datain) {
        return interface->instance->datain(pdev, epnum, interface->data);
    }

    return USBD_FAIL;
}

static uint8_t comp_dataout(struct _USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    usbd_interface_t* interface = find_interface_by_epnum(epnum);
    amk_printf("COMP DataOut: epnum=%d\n", epnum);
    
    if (interface && interface->instance->dataout) {
        return interface->instance->dataout(pdev, epnum, interface->data);
    }

    return USBD_FAIL;
}

bool usbd_comp_itf_ready(USBD_HandleTypeDef *pdev, uint32_t itf)
{
    if (pdev->dev_state != USBD_STATE_CONFIGURED) {
        return false;
    }

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        if (interface->index == itf) {
            return ((HID_HandleTypeDef *)interface->data)->state == ITF_IDLE;
        }
    }

    return false;
}

uint8_t usbd_comp_send(USBD_HandleTypeDef *pdev, uint8_t type, uint8_t *report, uint16_t len)
{
    if (pdev->dev_state != USBD_STATE_CONFIGURED) {
        return USBD_FAIL;
    }
    usbd_interface_t* interface = find_interface_by_type(type);
    static uint8_t send_buf[16];
    uint16_t send_len = len;
    switch(type) {
    case HID_REPORT_ID_KEYBOARD:
    case HID_REPORT_ID_VIAL: 
        memcpy(&send_buf[0], report, len);
        break;
    default:
        send_len++;
        send_buf[0] = type;
        memcpy(&send_buf[1], report, len);
        break;
    }

    if (interface && interface->instance->write) {
        //return interface->instance->write(pdev, interface->epin, report, len, interface->data);
        return interface->instance->write(pdev, interface->epin, send_buf, send_len, interface->data);
    }

    return USBD_FAIL;
}

// utilities
usbd_interface_t* find_interface_by_epnum(uint32_t epnum)
{
    if (epnum == 0) return NULL;

    for (int i = 0; i < usbd_composite.size; i++) {
        usbd_interface_t* interface = &usbd_composite.interfaces[i];
        if ((epnum==(interface->epin&0xFU)) || (epnum==interface->epout)) {
            return interface;
        }
    }

    return NULL;
}

usbd_interface_t* find_interface_by_type(uint32_t type)
{
    if (type == HID_REPORT_ID_KEYBOARD) {
        return &usbd_composite.interfaces[ITF_NUM_HID_KBD];
    } else if (type == HID_REPORT_ID_VIAL) {
        return &usbd_composite.interfaces[ITF_NUM_VIAL];
    } else {
        return &usbd_composite.interfaces[ITF_NUM_HID_OTHER];
    }
}

void hid_kbd_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_StatusTypeDef ret = USBD_OK;
    ret = USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    amk_printf("HID KBD init, Open epin=%d, status=%d\n", interface->epin, ret);
    pdev->ep_in[interface->epin & 0xFU].is_used = 1U;
    ((HID_HandleTypeDef *)interface->data)->state = ITF_IDLE;
    ((HID_HandleTypeDef *)interface->data)->type = HID_HANDLE_TYPE_KEYBOARD;
}

void hid_other_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_StatusTypeDef ret = USBD_OK;
    ret = USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    amk_printf("HID OTHER init, Open epin=%d, status=%d\n", interface->epin, ret);
    pdev->ep_in[interface->epin & 0xFU].is_used = 1U;
    ((HID_HandleTypeDef *)interface->data)->state = ITF_IDLE;
    ((HID_HandleTypeDef *)interface->data)->type = HID_HANDLE_TYPE_OTHER;
}

void hid_vial_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_StatusTypeDef ret = USBD_OK;
    ret = USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    amk_printf("HID VIAL init, Open epin=%d, status=%d\n", interface->epin, ret);
    pdev->ep_in[interface->epin & 0xFU].is_used = 1U;

    ret = USBD_LL_OpenEP(pdev, interface->epout, interface->epout_type, interface->epout_size);
    amk_printf("HID VIAL init, Open epout=%d, status=%d\n", interface->epout, ret);
    pdev->ep_out[interface->epout & 0xFU].is_used = 1U;
    ((HID_HandleTypeDef *)interface->data)->state = ITF_IDLE;
    ((HID_HandleTypeDef *)interface->data)->type = HID_HANDLE_TYPE_VIAL;


    ret = USBD_LL_PrepareReceive(pdev, interface->epout, ((HID_HandleTypeDef *)interface->data)->report, interface->epout_size);
    amk_printf("HID VIAL prepare receive, status=%d\n", ret);
}

void hid_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_LL_CloseEP(pdev, interface->epin);
    pdev->ep_in[interface->epin & 0xFU].is_used = 0U;
}

// ================================================================================
// HID class driver
// ================================================================================
static uint8_t  hid_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user)
{
    HID_HandleTypeDef *hhid = (HID_HandleTypeDef *)user;
    uint16_t len = 0U;
    uint8_t *pbuf = NULL;
    uint16_t status_info = 0U;
    USBD_StatusTypeDef ret = USBD_OK;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
        case USB_REQ_TYPE_CLASS :
            switch (req->bRequest) {
                case HID_REQ_SET_PROTOCOL:
                hhid->protocol = (uint8_t)(req->wValue);
                break;

                case HID_REQ_GET_PROTOCOL:
                USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->protocol, 1U);
                break;

                case HID_REQ_SET_IDLE:
                hhid->idle = (uint8_t)(req->wValue >> 8);
                break;

                case HID_REQ_GET_IDLE:
                USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->idle, 1U);
                break;
                case HID_REQ_SET_REPORT:
                amk_printf("HID SET REPORT: \n");
                USBD_CtlPrepareRx(pdev, &amk_led_state, 1U);
                break;

                default:
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
                break;
        } break;
        case USB_REQ_TYPE_STANDARD:
            switch (req->bRequest) {
                case USB_REQ_GET_STATUS:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    } break;
                case USB_REQ_GET_DESCRIPTOR:
                    if (req->wValue >> 8 == UDD_HID_DESC_TYPE_REPORT) {
                        if (hhid->type == HID_HANDLE_TYPE_KEYBOARD) {
                            len = udd_descriptor_hid_report_kbd_size();
                            amk_printf("keyboard report desc size: %d\n", len);
                            pbuf = (uint8_t*)udd_descriptor_hid_report_kbd();
                        } else if (hhid->type == HID_HANDLE_TYPE_OTHER) {
                            len = udd_descriptor_hid_report_other_size();
                            amk_printf("other report desc size: %d\n", len);
                            pbuf = (uint8_t*)udd_descriptor_hid_report_other();
                        } else {
                        #ifdef VIAL_ENABLE
                            len = udd_descriptor_hid_report_vial_size();
                            amk_printf("vial report desc size: %d\n", len);
                            pbuf = (uint8_t*)udd_descriptor_hid_report_vial();
                        #endif
                        }
                    } else if (req->wValue >> 8 == UDD_HID_DESC_TYPE_HID) {
                        if (hhid->type == HID_HANDLE_TYPE_KEYBOARD) {
                            len = udd_descriptor_hid_interface_kbd_size();
                            pbuf = (uint8_t*)udd_descriptor_hid_interface_kbd();
                        } else if (hhid->type == HID_HANDLE_TYPE_OTHER) {
                            len = udd_descriptor_hid_interface_other_size();
                            pbuf = (uint8_t*)udd_descriptor_hid_interface_other();
                        } else {
                            len = udd_descriptor_hid_interface_vial_size();
                            pbuf = (uint8_t*)udd_descriptor_hid_interface_vial();
                        #ifdef VIAL_ENABLE
                        #endif
                        }
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                        break;
                    }
                    USBD_CtlSendData(pdev, pbuf, len);
                    break;
                case USB_REQ_GET_INTERFACE :
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->alt, 1U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_SET_INTERFACE :
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        hhid->alt = (uint8_t)(req->wValue);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                break;
            } break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
        }

    return ret;
}

static uint8_t hid_datain(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    HID_HandleTypeDef * handle= (HID_HandleTypeDef *)user;
    handle->state = ITF_IDLE;

#ifdef VIAL_ENABLE
    if (handle->type == HID_HANDLE_TYPE_VIAL) {
        USBD_StatusTypeDef ret = USBD_OK;
        usbd_interface_t* intf = find_interface_by_epnum(epnum);
        ret = USBD_LL_PrepareReceive(pdev, intf->epout, handle->report, USBD_HID_VIAL_EPOUT_SIZE);
        amk_printf("HID VIAL prepare receive, status=%d\n", ret);
    }
#endif
    return USBD_OK;
}

static uint8_t  hid_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    HID_HandleTypeDef *handle = (HID_HandleTypeDef *)user;
#ifdef VIAL_ENABLE
    if (handle->type == HID_HANDLE_TYPE_VIAL) {
        amk_printf("USBD HID dataout: \n");
        vial_process(handle->report, USBD_HID_VIAL_EPOUT_SIZE);
    }
#endif
    return USBD_OK;
}

static uint8_t  hid_write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user)
{
    HID_HandleTypeDef* hhid = (HID_HandleTypeDef*)user;
    amk_printf("USBD HID Write: state=%d, epnum=%d, size=%d\n", hhid->state, epnum, size);
    if (hhid->state == ITF_IDLE) {
        hhid->state = ITF_BUSY;
        return USBD_LL_Transmit(pdev, epnum, data, size);
    } else {
        return USBD_BUSY;
    }
}

// ================================================================================
// Mass storage class driver
// ================================================================================
#ifdef MSC_ENABLE
static void msc_init(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    USBD_StatusTypeDef ret = USBD_OK;
    ret = USBD_LL_OpenEP(pdev, interface->epin, interface->epin_type, interface->epin_size);
    amk_printf("MSC init, Open epin=%d, status=%d\n", interface->epin, ret);
    pdev->ep_in[interface->epin & 0xFU].is_used = 1U;
    ret = USBD_LL_OpenEP(pdev, interface->epout, interface->epout_type, interface->epout_size);
    amk_printf("MSC init, Open epout=%d, status=%d\n", interface->epout, ret);
    pdev->ep_out[interface->epout].is_used = 1U;

    MSC_BOT_Init(pdev, interface->data);
}

static void msc_uninit(USBD_HandleTypeDef* pdev, usbd_interface_t* interface)
{
    (void)USBD_LL_CloseEP(pdev, interface->epin);
    pdev->ep_in[interface->epin & 0xFU].is_used = 0U;

    (void)USBD_LL_CloseEP(pdev, interface->epout);
    pdev->ep_out[interface->epout].is_used = 0U;

    MSC_BOT_DeInit(pdev, interface->data);
}

extern USBD_StorageTypeDef storage_ops;

static uint8_t  msc_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user)
{
    USBD_MSC_BOT_HandleTypeDef *hmsc = (USBD_MSC_BOT_HandleTypeDef *)user;
    USBD_StatusTypeDef ret = USBD_OK;
    uint16_t status_info = 0U;

    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
        /* Class request */
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest)
        {
        case BOT_GET_MAX_LUN:
        if ((req->wValue  == 0U) && (req->wLength == 1U) &&
            ((req->bmRequest & 0x80U) == 0x80U))
        {
            hmsc->max_lun = (uint32_t)storage_ops.GetMaxLun();
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmsc->max_lun, 1U);
        }
        else
        {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
        }
        break;

        case BOT_RESET :
        if ((req->wValue  == 0U) && (req->wLength == 0U) &&
            ((req->bmRequest & 0x80U) != 0x80U))
        {
            MSC_BOT_Reset(pdev, hmsc);
        }
        else
        {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
        }
        break;

        default:
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
        break;
        }
        break;
        /* Interface & Endpoint request */
    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest)
        {
        case USB_REQ_GET_STATUS:
        if (pdev->dev_state == USBD_STATE_CONFIGURED)
        {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
        }
        else
        {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
        }
        break;

        case USB_REQ_GET_INTERFACE:
        if (pdev->dev_state == USBD_STATE_CONFIGURED)
        {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmsc->interface, 1U);
        }
        else
        {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
        }
        break;

        case USB_REQ_SET_INTERFACE:
        if (pdev->dev_state == USBD_STATE_CONFIGURED)
        {
            hmsc->interface = (uint8_t)(req->wValue);
        }
        else
        {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
        }
        break;

        case USB_REQ_CLEAR_FEATURE:
        if (pdev->dev_state == USBD_STATE_CONFIGURED)
        {
            if (req->wValue == USB_FEATURE_EP_HALT)
            {
            /* Flush the FIFO */
            (void)USBD_LL_FlushEP(pdev, (uint8_t)req->wIndex);

            /* Handle BOT error */
            MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex, hmsc);
            }
        }
        break;

        default:
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
        break;
        }
        break;

    default:
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
        break;
    }

    return (uint8_t)ret;
}

static uint8_t  msc_datain(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    MSC_BOT_DataIn(pdev, epnum, user);
    return (uint8_t)USBD_OK;
}

static uint8_t  msc_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    MSC_BOT_DataOut(pdev, epnum, user);
    return (uint8_t)USBD_OK;
}

static uint8_t  msc_write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user)
{
    return (uint8_t)USBD_OK;
}

#include "usbd_msc_bot.c"
#include "usbd_msc_scsi.c"

#endif