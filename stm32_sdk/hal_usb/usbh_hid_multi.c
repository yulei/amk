/**
 * @file usbdh_hid_multi.c
 * 
 */

#include "usbh_hid_multi.h"
#include "report_parser.h"
#include "amk_printf.h"

// buffer for store the report descriptor
#define REPORT_DESC_BUF_SIZE 256
typedef struct {
    uint32_t data[REPORT_DESC_BUF_SIZE];
    uint32_t allocated;
    uint32_t total;
} report_desc_buf_t;

static void report_desc_buf_init(report_desc_buf_t* buf);
static uint8_t* report_desc_buf_alloc(report_desc_buf_t* buf, uint32_t size);

static USBH_StatusTypeDef hid_multi_init(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_deinit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_request(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_sof(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef hid_itf_init(USBH_HandleTypeDef *phost, uint8_t itf, USBH_InterfaceDescTypeDef *desc);
static USBH_StatusTypeDef hid_itf_deinit(USBH_HandleTypeDef *phost, uint8_t itf);
static USBH_StatusTypeDef hid_itf_request(USBH_HandleTypeDef *phost, uint8_t itf);
static USBH_StatusTypeDef hid_itf_process(USBH_HandleTypeDef *phost, uint8_t itf);
static USBH_StatusTypeDef hid_itf_sof(USBH_HandleTypeDef *phost, uint8_t itf);

static void hid_desc_parse(HID_DescTypeDef *desc, uint8_t *buf);

USBH_ClassTypeDef  HID_Multi_Class = {
    "HID Multi",
    USB_HID_CLASS,
    hid_multi_init,
    hid_multi_deinit,
    hid_multi_request,
    hid_multi_process,
    hid_multi_sof,
    NULL,
};

static HID_HandleTypeDef hid_multi;
static report_desc_buf_t report_desc_buffer;

static void report_desc_buf_init(report_desc_buf_t* buf)
{
    memset(&buf->data[0], 0, sizeof(uint32_t)*REPORT_DESC_BUF_SIZE);
    buf->total = sizeof(uint32_t)*REPORT_DESC_BUF_SIZE;
    buf->allocated = 0;
}

static uint8_t* report_desc_buf_alloc(report_desc_buf_t* buf, uint32_t size)
{
    uint32_t aligned = (size+sizeof(uint32_t)-1) / sizeof(uint32_t);
    if ((aligned+buf->allocated) > buf->total) {
        amk_printf("Report descriptor buffer allocated failed: total=%d, allocated=%d, required=%d\n", buf->total, buf->allocated, aligned);
        return NULL;
    }

    uint8_t* p = (uint8_t*)(&buf->data[buf->allocated]);
    buf->allocated += aligned;
    return p;
}

static USBH_StatusTypeDef hid_multi_init(USBH_HandleTypeDef *phost)
{
    USBH_StatusTypeDef status = USBH_FAIL;
    HID_HandleTypeDef *phid = &hid_multi;

    memset(phid, 0, sizeof(HID_HandleTypeDef));
    phost->pActiveClass->pData = phid;
    phid->host = phost;
    report_desc_buf_init(&report_desc_buffer);

    USBH_CfgDescTypeDef *pcfg = &phost->device.CfgDesc;
    phid->itf_num = pcfg->bNumInterfaces;
    for (int i = 0; i < phid->itf_num; i++) {
        USBH_InterfaceDescTypeDef *desc = &pcfg->Itf_Desc[i];
        if (desc->bInterfaceClass == USB_HID_CLASS) {
            hid_itf_init(phost, i, desc);
            status = USBH_OK;
        }
    }

    phid->inited = 1; 
    amk_printf("HID multiclass inited: itfnum=%d\n", phid->itf_num);
    return status;
}

static USBH_StatusTypeDef hid_multi_deinit(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            hid_itf_deinit(phost, i);
        }
    }

    report_desc_buf_init(&report_desc_buffer);
    memset(phid, 0, sizeof(HID_HandleTypeDef));
    phost->pActiveClass->pData = 0U;
    amk_printf("HID multiclass deinited\n");
    return USBH_OK;
}

static USBH_StatusTypeDef hid_multi_request(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid == 0) {
            hid_itf_request(phost, i);
            break;
        }
    }

    // need to wait until all interface were initialized
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid == 0) {
            return USBH_BUSY;
        }
    }

    amk_printf("HID multi class request all done\n");
    return USBH_OK;
}

static USBH_StatusTypeDef hid_multi_process(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            hid_itf_process(phost, i);
        }
    }
    return USBH_OK;
}

static USBH_StatusTypeDef hid_multi_sof(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            hid_itf_sof(phost, i);
        }
    }

    return USBH_OK;
}

static USBH_StatusTypeDef hid_itf_init(USBH_HandleTypeDef *phost, uint8_t itf, USBH_InterfaceDescTypeDef* desc)
{
    USBH_StatusTypeDef status = USBH_OK;
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef * pitf = &phid->itfs[itf];

    pitf->state         = HID_INIT;
    pitf->ctl_state     = HID_REQ_INIT;
    pitf->packet_size   = desc->Ep_Desc[0].wMaxPacketSize;
    pitf->poll          = desc->Ep_Desc[0].bInterval;
    USBH_HID_FifoInit(&pitf->fifo, pitf->fifo_buf, HID_QUEUE_SIZE*HID_PACKET_SIZE);

    if (pitf->poll < HID_MIN_POLL) {
        pitf->poll = HID_MIN_POLL;
    }
    uint8_t max_ep = desc->bNumEndpoints;
    /* Decode endpoint IN and OUT address from interface descriptor */
    for ( int i = 0; i < max_ep; i++) {
        if (desc->Ep_Desc[i].bEndpointAddress & 0x80U) {
            // in endpoint
            pitf->in_ep= desc->Ep_Desc[i].bEndpointAddress;
            pitf->in_pipe= USBH_AllocPipe(phost, pitf->in_ep);
            /* Open pipe for IN endpoint */
            USBH_OpenPipe(phost, pitf->in_pipe, pitf->in_ep, phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, pitf->packet_size);
            USBH_LL_SetToggle(phost, pitf->in_pipe, 0U);
        } else {
            pitf->out_ep = desc->Ep_Desc[i].bEndpointAddress;
            pitf->out_pipe = USBH_AllocPipe(phost, pitf->out_ep);
            /* Open pipe for OUT endpoint */
            USBH_OpenPipe(phost, pitf->out_pipe, pitf->out_ep, phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, pitf->packet_size);
            USBH_LL_SetToggle(phost, pitf->out_pipe, 0U);
        }
    }
    amk_printf("HID interface [%d] inited\n", itf);
    return status;
}

static USBH_StatusTypeDef hid_itf_deinit(USBH_HandleTypeDef *phost, uint8_t itf)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *pitf = &phid->itfs[itf];
    if (pitf->in_pipe!= 0x00U) {
        USBH_ClosePipe(phost, pitf->in_pipe);
        USBH_FreePipe(phost, pitf->in_pipe);
        pitf->in_pipe = 0U;
    }
    
    if (pitf->out_pipe != 0x00U) {
        USBH_ClosePipe(phost, pitf->out_pipe);
        USBH_FreePipe(phost, pitf->out_pipe);
        pitf->out_pipe = 0U;
    }

    amk_printf("HID interface [%d] deinited\n", itf);
    return USBH_OK;
}

static USBH_StatusTypeDef hid_itf_request(USBH_HandleTypeDef *phost, uint8_t itf)
{
    USBH_StatusTypeDef status         = USBH_BUSY;
    USBH_StatusTypeDef classReqStatus = USBH_BUSY;
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *pitf = &phid->itfs[itf];

    /* Switch HID state machine */
    switch (pitf->ctl_state) {
    case HID_REQ_INIT:
    case HID_REQ_GET_HID_DESC: 
        hid_desc_parse(&pitf->hid_desc, phost->device.CfgDesc_Raw);
        pitf->report_desc = report_desc_buf_alloc(&report_desc_buffer, pitf->hid_desc.wDescriptorLength);
        pitf->ctl_state = HID_REQ_GET_REPORT_DESC;
        break;
    case HID_REQ_GET_REPORT_DESC:
        classReqStatus = USBH_HID_GetHIDReportDescriptor(phost, pitf->hid_desc.wDescriptorLength, itf);
        if (classReqStatus == USBH_OK) {
            pitf->type = HID_UNKNOWN;
            if (rp_parse(itf, pitf->report_desc, pitf->hid_desc.wDescriptorLength)) {
                itf_report_desc_t * ird = rp_get(itf);
                if (ird->desc_count == 1) {
                    switch (ird->desc[0].rd_type) {
                        case RDI_KEYBOARD:
                            pitf->type = HID_KEYBOARD;
                            break;
                        case RDI_MOUSE:
                            pitf->type = HID_MOUSE;
                            break;
                        case RDI_RAW:
                            pitf->type = HID_RAW;
                            break;
                        default:
                            break;
                    }
                } else {
                    pitf->type = HID_EXTRA;
                }
            }
            pitf->ctl_state = HID_REQ_SET_IDLE;
            amk_printf("HID interface[%d]: get report descriptor, type=%d\n", itf, pitf->type);
        } else if (classReqStatus == USBH_NOT_SUPPORTED) {
            amk_printf("HID interface[%d]: failed to get report descriptor\n", itf);
            status = USBH_FAIL;
        } else {
        }
        break;
    case HID_REQ_SET_IDLE:
        classReqStatus = USBH_HID_SetIdle(phost, 0U, 0U, itf);
        if (classReqStatus == USBH_OK) {
            pitf->ctl_state = HID_REQ_SET_PROTOCOL;
        } else {
            if (classReqStatus == USBH_NOT_SUPPORTED) {
                pitf->ctl_state = HID_REQ_SET_PROTOCOL;
            }
        }
        break;
    case HID_REQ_SET_PROTOCOL:
        classReqStatus = USBH_HID_SetProtocol(phost, 0U, itf);
        if (classReqStatus == USBH_OK) {
            pitf->ctl_state = HID_REQ_IDLE;
            pitf->valid = 1;

            /* all requests performed*/
            phost->pUser(phost, HOST_USER_CLASS_ACTIVE);
            status = USBH_OK;
        } else if (classReqStatus == USBH_NOT_SUPPORTED) {
            amk_printf("HID interface[%]: failed to set protocol\n", itf);
            status = USBH_FAIL;
        } else {
        }
        break;
    case HID_REQ_IDLE:
    default:
        break;
    }

    return status;
}

#if (USBH_USE_OS == 1U) 
    #if (osCMSIS < 0x20000U) 
        #define PUT_EVENT(a, b) \
            phost->os_msg = (uint32_t)USBH_URB_EVENT; \
            (void)osMessagePut(phost->os_event, phost->os_msg, (a));
    #else
    #define PUT_EVENT(a, b) \
            phost->os_msg = (uint32_t)USBH_URB_EVENT; \
            (void)osMessageQueuePut(phost->os_event, &phost->os_msg, (a), (b));
    #endif
#else
    #define PUT_EVENT(a, b)
#endif

static USBH_StatusTypeDef hid_itf_process(USBH_HandleTypeDef *phost, uint8_t itf)
{
    USBH_StatusTypeDef status = USBH_OK;
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *pitf = &phid->itfs[itf];

    switch (pitf->state) {
    case HID_INIT:
        if (phid->inited == 0) {
          phid->Init(phost);
        }
        //pitf->state = HID_IDLE;
        pitf->state = HID_GET_DATA;

        PUT_EVENT(0U, NULL);
    break;

    case HID_IDLE:
        status = USBH_HID_GetReport(phost, 0x01, 0, &pitf->report_buf[0], (uint8_t)pitf->packet_size, itf);
        if (status == USBH_OK) {
            pitf->state = HID_SYNC;
        } else if (status == USBH_BUSY) {
            pitf->state = HID_IDLE;
            status = USBH_OK;
        } else if (status == USBH_NOT_SUPPORTED) {
            pitf->state = HID_SYNC;
            status = USBH_OK;
        } else {
            pitf->state = HID_ERROR;
            status = USBH_FAIL;
        }

        PUT_EVENT(0U, NULL);
        break;

    case HID_SYNC:
        if (phost->Timer & 1U) {
            pitf->state = HID_GET_DATA;
        }

        PUT_EVENT(0U, NULL);
        break;

    case HID_GET_DATA:
        USBH_InterruptReceiveData(phost, pitf->report_buf, (uint8_t)pitf->packet_size, pitf->in_pipe); 
        //amk_printf("start receving data: itf=%d, size=%d, in_pipe=%d\n", itf, pitf->packet_size, pitf->in_pipe);
        pitf->state = HID_POLL;
        pitf->timer = phost->Timer;
        pitf->data_ready = 0U;
        break;

    case HID_POLL:
        if (USBH_LL_GetURBState(phost, pitf->in_pipe) == USBH_URB_DONE) {
            uint32_t xfer_size = USBH_LL_GetLastXferSize(phost, pitf->in_pipe);
            if ((pitf->data_ready == 0U) && (xfer_size!= 0U)) {
                pitf->report_size = xfer_size; 
                USBH_HID_FifoWrite(&pitf->fifo, pitf->report_buf, pitf->report_size);
                pitf->data_ready = 1U;
                phid->current = itf;
                USBH_HID_EventCallback(phost);
                PUT_EVENT(0U, NULL);
            }
        } else {
            if (USBH_LL_GetURBState(phost, pitf->in_pipe) == USBH_URB_STALL) {
                /* Issue Clear Feature on interrupt IN endpoint */
                if (USBH_ClrFeature(phost, pitf->in_ep) == USBH_OK) {
                    /* Change state to issue next IN token */
                    pitf->state = HID_GET_DATA;
                }
            }
        }
        break;
      default:
        break;
    }
    return status;
}

static USBH_StatusTypeDef hid_itf_sof(USBH_HandleTypeDef *phost, uint8_t itf)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *pitf = &phid->itfs[itf];
    if (pitf->state == HID_POLL) {
        if ((phost->Timer - pitf->timer) >= pitf->poll) {
            pitf->state = HID_GET_DATA;

            PUT_EVENT(0U, NULL);
        }
    }
    return USBH_OK;
}

USBH_StatusTypeDef USBH_HID_GetHIDReportDescriptor(USBH_HandleTypeDef *phost, uint16_t length, uint8_t itf)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *pitf = &phid->itfs[itf];

    if (phost->RequestState == CMD_SEND)
    {
        phost->Control.setup.b.bmRequestType  = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD;
        phost->Control.setup.b.bRequest       = USB_REQ_GET_DESCRIPTOR;
        phost->Control.setup.b.wValue.w       = USB_DESC_HID_REPORT;
        phost->Control.setup.b.wIndex.w       = itf;
        phost->Control.setup.b.wLength.w      = length;
    }

    return USBH_CtlReq(phost, pitf->report_desc, length);
}

USBH_StatusTypeDef USBH_HID_GetHIDDescriptor(USBH_HandleTypeDef *phost, uint16_t length, uint8_t itf)
{
    if (phost->RequestState == CMD_SEND)
    {
        phost->Control.setup.b.bmRequestType  = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD;
        phost->Control.setup.b.bRequest       = USB_REQ_GET_DESCRIPTOR;
        phost->Control.setup.b.wValue.w       = USB_DESC_HID;
        phost->Control.setup.b.wIndex.w       = itf;
        phost->Control.setup.b.wLength.w      = length;
    }

    return USBH_CtlReq(phost, phost->device.Data, length);
}

USBH_StatusTypeDef USBH_HID_SetIdle(USBH_HandleTypeDef *phost, uint8_t duration, uint8_t reportId, uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS;


    phost->Control.setup.b.bRequest = USB_HID_SET_IDLE;
    phost->Control.setup.b.wValue.w = (uint16_t)(((uint32_t)duration << 8U) | (uint32_t)reportId);

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = 0U;

    return USBH_CtlReq(phost, 0U, 0U);
}


USBH_StatusTypeDef USBH_HID_SetReport(USBH_HandleTypeDef *phost, uint8_t type, uint8_t id, uint8_t *report, uint8_t length, uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS;


    phost->Control.setup.b.bRequest = USB_HID_SET_REPORT;
    phost->Control.setup.b.wValue.w = (uint16_t)(((uint32_t)type << 8U) | (uint32_t)id);

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = length;

    return USBH_CtlReq(phost, report, (uint16_t)length);
}


USBH_StatusTypeDef USBH_HID_GetReport(USBH_HandleTypeDef *phost, uint8_t type, uint8_t id, uint8_t *report, uint8_t length, uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS;
    phost->Control.setup.b.bRequest = USB_HID_GET_REPORT;
    phost->Control.setup.b.wValue.w = (uint16_t)(((uint32_t)type << 8U) | (uint32_t)id);

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = length;

    return USBH_CtlReq(phost, report, (uint16_t)length);
}

USBH_StatusTypeDef USBH_HID_SetProtocol(USBH_HandleTypeDef *phost, uint8_t protocol, uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS;

    phost->Control.setup.b.bRequest = USB_HID_SET_PROTOCOL;
    if (protocol) {
        phost->Control.setup.b.wValue.w = 0U;
    } else {
        phost->Control.setup.b.wValue.w = 1U;
    }

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = 0U;

    return USBH_CtlReq(phost, 0U, 0U);
}

static void  hid_desc_parse(HID_DescTypeDef *desc, uint8_t *buf)
{
    USBH_DescHeader_t *pdesc = (USBH_DescHeader_t *)buf;
    uint16_t CfgDescLen;
    uint16_t ptr;

    CfgDescLen = LE16(buf + 2U);

    if (CfgDescLen > USB_CONFIGURATION_DESC_SIZE) {
        ptr = USB_LEN_CFG_DESC;

        while (ptr < CfgDescLen) {
            pdesc = USBH_GetNextDesc((uint8_t *)pdesc, &ptr);

            if (pdesc->bDescriptorType == USB_DESC_TYPE_HID) {
                desc->bLength = *(uint8_t *)((uint8_t *)pdesc + 0U);
                desc->bDescriptorType = *(uint8_t *)((uint8_t *)pdesc + 1U);
                desc->bcdHID = LE16((uint8_t *)pdesc + 2U);
                desc->bCountryCode = *(uint8_t *)((uint8_t *)pdesc + 4U);
                desc->bNumDescriptors = *(uint8_t *)((uint8_t *)pdesc + 5U);
                desc->bReportDescriptorType = *(uint8_t *)((uint8_t *)pdesc + 6U);
                desc->wDescriptorLength = LE16((uint8_t *)pdesc + 7U);
                break;
            }
        }
    }
}

HID_TypeTypeDef USBH_HID_GetDeviceType(USBH_HandleTypeDef *phost, uint8_t itf)
{
    if (phost->gState == HOST_CLASS) {
        HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
        HID_InterfaceTypeDef *pitf = &phid->itfs[itf];
        return pitf->type;
    } else {
        return HID_UNKNOWN;
    }
}

uint8_t USBH_HID_GetPollInterval(USBH_HandleTypeDef *phost, uint8_t itf)
{
    if ((phost->gState == HOST_CLASS_REQUEST) ||
        (phost->gState == HOST_INPUT) ||
        (phost->gState == HOST_SET_CONFIGURATION) ||
        (phost->gState == HOST_CHECK_CLASS) ||
        (phost->gState == HOST_CLASS)) {

        HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
        HID_InterfaceTypeDef *pitf = &phid->itfs[itf];
        return (uint8_t)(pitf->poll);
    } else {
      return 0U;
    }
}

/**
  * @brief  USBH_HID_FifoInit
  *         Initialize FIFO.
  * @param  f: Fifo address
  * @param  buf: Fifo buffer
  * @param  size: Fifo Size
  * @retval none
  */
void USBH_HID_FifoInit(FIFO_TypeDef *f, uint8_t *buf, uint16_t size)
{
    f->head = 0U;
    f->tail = 0U;
    f->lock = 0U;
    f->size = size;
    f->buf  = buf;
}

/**
  * @brief  USBH_HID_FifoRead
  *         Read from FIFO.
  * @param  f: Fifo address
  * @param  buf: read buffer
  * @param  nbytes: number of item to read
  * @retval number of read items
  */
uint16_t USBH_HID_FifoRead(FIFO_TypeDef *f, void *buf, uint16_t nbytes)
{
    uint16_t i;
    uint8_t *p;

    p = (uint8_t *) buf;

    if (f->lock == 0U) {
        f->lock = 1U;

        for (i = 0U; i < nbytes; i++) {
            if (f->tail != f->head) {
                *p++ = f->buf[f->tail];
                f->tail++;

                if (f->tail == f->size) {
                    f->tail = 0U;
                }
            } else {
                f->lock = 0U;
                return i;
            }
        }
    }

    f->lock = 0U;

    return nbytes;
}

/**
  * @brief  USBH_HID_FifoWrite
  *         Write To FIFO.
  * @param  f: Fifo address
  * @param  buf: read buffer
  * @param  nbytes: number of item to write
  * @retval number of written items
  */
uint16_t USBH_HID_FifoWrite(FIFO_TypeDef *f, void *buf, uint16_t  nbytes)
{
    uint16_t i;
    uint8_t *p;

    p = (uint8_t *) buf;

    if (f->lock == 0U) {
        f->lock = 1U;

        for (i = 0U; i < nbytes; i++) {
            if ((f->head + 1U == f->tail) || ((f->head + 1U == f->size) && (f->tail == 0U))) {
                f->lock = 0U;
                return i;
            } else {
                f->buf[f->head] = *p++;
                f->head++;

                if (f->head == f->size) {
                    f->head = 0U;
                }
            }
        }
    }

    f->lock = 0U;
    return nbytes;
}

/**
* @brief  The function is a callback about HID Data events
*  @param  phost: Selected device
* @retval None
*/
__weak void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(phost);
}