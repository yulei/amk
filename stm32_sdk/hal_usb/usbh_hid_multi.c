/**
 * @file usbdh_hid_multi.c
 * 
 */

#include "usbh_hid_multi.h"
#include "report_parser.h"

static USBH_StatusTypeDef hid_multi_init(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_deinit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_class_request(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef hid_multi_sof_process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef hid_itf_init(USBH_HandleTypeDef *phost, uint8_t itf, USBH_InterfaceDescTypeDef *desc);
static USBH_StatusTypeDef hid_itf_deinit(USBH_HandleTypeDef *phost, uint8_t itf);
static USBH_StatusTypeDef hid_itf_request(USBH_HandleTypeDef *phost, uint8_t itf);
static USBH_StatusTypeDef hid_itf_process(USBH_HandleTypeDef *phost, uint8_t itf);
static USBH_StatusTypeDef hid_itf_sof(USBH_HandleTypeDef *phost, uint8_t itf);

static void hid_parse_desc(HID_DescTypeDef *desc, uint8_t *buf);
//static itf_report_desc_t *hid_parse_report_desc(uint8_t *buf, size_t size);

USBH_ClassTypeDef  HID_Multi_Class = {
    "HID Multi",
    USB_HID_CLASS,
    hid_multi_init,
    hid_multi_deinit,
    hid_multi_class_request,
    hid_multi_process,
    hid_multi_sof_process,
    NULL,
};

static HID_HandleTypeDef hid_multi;

static USBH_StatusTypeDef hid_multi_init(USBH_HandleTypeDef *phost)
{
    USBH_StatusTypeDef status = USBH_FAIL;
    HID_HandleTypeDef *phid = &hid_multi;

    memset(phid, 0, sizeof(HID_HandleTypeDef));
    phost->pActiveClass->pData = phid;

    USBH_CfgDescTypeDef *pcfg = &phost->device.CfgDesc;
    phid->itf_num = pcfg->bNumInterfaces;
    for (int i = 0; i < phid->itf_num; i++) {
        USBH_InterfaceDescTypeDef *desc = &pcfg->Itf_Desc[i];
        if (desc->bInterfaceClass == USB_HID_CLASS) {
            USBH_HID_InterfaceInit(phost, i, desc);
            status = USBH_OK;
        }
    }

    phid->inited = 1; 
    return status;
}

static USBH_StatusTypeDef hid_multi_deinit(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            USBH_HID_InterfaceDeInit(phost, i);
        }
    }

    memset(phid, 0, sizeof(HID_HandleTypeDef));
    phost->pActiveClass->pData = 0U;
    return USBH_OK;
}

static USBH_StatusTypeDef hid_multi_class_request(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            USBH_HID_ClassRequest(phost, i);
        }
    }

    return USBH_OK;
}

static USBH_StatusTypeDef hid_multi_process(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            USBH_HID_PRocess(phost, i);
        }
    }
    return USBH_OK;
}

static USBH_StatusTypeDef hid_multi_sof_process(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef*)phost->pActiveClass->pData;
    for (int i = 0; i < phid->itf_num; i++) {
        if (phid->itfs[i].valid != 0) {
            USBH_HID_ClassRequest(phost, i);
        }
    }
    return USBH_OK;
}

static USBH_StatusTypeDef hid_itf_init(USBH_HandleTypeDef *phost, uint8_t itf, USBH_InterfaceDescTypeDef* desc)
{
    USBH_StatusTypeDef status = USBH_OK;
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef * pitf = &phid->itfs[itf];

    pitf->state     = HID_INIT;
    pitf->ctl_state = HID_REQ_INIT;
    pitf->ep_addr   = desc->Ep_Desc[0].bEndpointAddress;
    pitf->length    = desc->Ep_Desc[0].wMaxPacketSize;
    pitf->poll      = desc->Ep_Desc[0].bInterval;
    if (pitf->poll < HID_MIN_POLL) {
        pitf->poll = HID_MIN_POLL;
    }
    uint8_t max_ep = desc->bNumEndpoints;
    /* Decode endpoint IN and OUT address from interface descriptor */
    for ( int i = 0; i < desc->bNumEndpoints; i++) {
        if (desc->Ep_Desc[i].bEndpointAddress & 0x80U) {
            // in endpoint
            pitf->InEp = desc->Ep_Desc[i].bEndpointAddress;
            pitf->InPipe = USBH_AllocPipe(phost, pitf->InEp);
            /* Open pipe for IN endpoint */
            USBH_OpenPipe(phost, pitf->InPipe, pitf->InEp, phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, pitf->length);
            USBH_LL_SetToggle(phost, pitf->InPipe, 0U);
        } else {
            pitf->OutEp = desc->Ep_Desc[i].bEndpointAddress;
            pitf->OutPipe = USBH_AllocPipe(phost, pitf->OutEp);
            /* Open pipe for OUT endpoint */
            USBH_OpenPipe(phost, pitf->OutPipe, pitf->OutEp, phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, pirf->length);
            USBH_LL_SetToggle(phost, pirf->OutPipe, 0U);
        }
    }
    return USBH_OK;
}

static USBH_StatusTypeDef hid_itf_deinit(USBH_HandleTypeDef *phost, uint8_t itf)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *pitf = &phid->itfs[itf];
    if (pitf->InPipe != 0x00U) {
        USBH_ClosePipe(phost, pitf->InPipe);
        USBH_FreePipe(phost, pitf->InPipe);
        pitf->InPipe = 0U;
    }
    
    if (pitf->OutPipe != 0x00U) {
        USBH_ClosePipe(phost, pitf->OutPipe);
        USBH_FreePipe(phost, pitf->OutPipe);
        pitf->OutPipe = 0U;
    }

    /*if (phost->pActiveClass->pData)
    {
        USBH_free(phost->pActiveClass->pData);
        phost->pActiveClass->pData = 0U;
    }*/
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
        USBH_HID_ParseHIDDesc(&pitf->HID_Desc, phost->device.CfgDesc_Raw);
        pitf->ctl_state = HID_REQ_GET_REPORT_DESC;
        break;
    case HID_REQ_GET_REPORT_DESC:
        classReqStatus = USBH_HID_GetHIDReportDescriptor(phost, pitf->HID_Desc.wItemLength, itf);
        if (classReqStatus == USBH_OK) {
            pitf->ctl_state = HID_REQ_SET_IDLE;
        } else if (classReqStatus == USBH_NOT_SUPPORTED) {
            amk_printf("Control error: HID: Device Get Report Descriptor request failed\n");
            status = USBH_FAIL;
        } else {

        }
        break;
    case HID_REQ_SET_IDLE:
        classReqStatus = USBH_HID_SetIdle(phost, 0U, 0U);
        if (classReqStatus == USBH_OK) {
            pitf->ctl_state = HID_REQ_SET_PROTOCOL;
        } else {
            if (classReqStatus == USBH_NOT_SUPPORTED) {
                pitf->ctl_state = HID_REQ_SET_PROTOCOL;
            }
        }
        break;
    case HID_REQ_SET_PROTOCOL:
        classReqStatus = USBH_HID_SetProtocol(phost, 0U);
        if (classReqStatus == USBH_OK) {
            pitf->ctl_state = HID_REQ_IDLE;

            /* all requests performed*/
            phost->pUser(phost, HOST_USER_CLASS_ACTIVE);
            status = USBH_OK;
        } else if (classReqStatus == USBH_NOT_SUPPORTED) {
            amk_printf("Control error: HID: Device Set protocol request failed\n");
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

#define PUT_EVENT(a, b) \
    #if (USBH_USE_OS == 1U) \
        phost->os_msg = (uint32_t)USBH_URB_EVENT; \
    #if (osCMSIS < 0x20000U) \
        (void)osMessagePut(phost->os_event, phost->os_msg, (a)); \
    #else \
        (void)osMessageQueuePut(phost->os_event, &phost->os_msg, (a), (b)); \
    #endif \
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
        pitf->state = HID_IDLE;

        PUT_EVENT(0U, NULL);
    break;

    case HID_IDLE:
        status = USBH_HID_GetReport(phost, 0x01U, 0U, pitf->pData, (uint8_t)pitf->length);
        if (status == USBH_OK) {
            pitf->state = HID_SYNC;
        } else if (status == USBH_BUSY) {
            HID_Handle->state = HID_IDLE;
            status = USBH_OK;
        } else if (status == USBH_NOT_SUPPORTED) {
            HID_Handle->state = HID_SYNC;
            status = USBH_OK;
        } else {
            HID_Handle->state = HID_ERROR;
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
        USBH_InterruptReceiveData(phost, pitf->pData, (uint8_t)pitf->length, pitf->InPipe); 
        pitf->state = HID_POLL;
        pitf->timer = phost->Timer;
        pitf->DataReady = 0U;
        break;

    case HID_POLL:
        if (USBH_LL_GetURBState(phost, pitf->InPipe) == USBH_URB_DONE) {
            uint32_t XferSize = USBH_LL_GetLastXferSize(phost, pitf->InPipe);
            if ((pitf->DataReady == 0U) && (XferSize != 0U)) {
                USBH_HID_FifoWrite(&HID_Handle->fifo, HID_Handle->pData, HID_Handle->length);
                HID_Handle->DataReady = 1U;
                USBH_HID_EventCallback(phost);
                PUT_EVENT(0U, NULL);
            }
        } else {
            if (USBH_LL_GetURBState(phost, pitf->InPipe) == USBH_URB_STALL) {
                /* Issue Clear Feature on interrupt IN endpoint */
                if (USBH_ClrFeature(phost, pitf->ep_addr) == USBH_OK) {
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
    if (phost->RequestState == CMD_SEND)
    {
        phost->Control.setup.b.bmRequestType  = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD;
        phost->Control.setup.b.bRequest       = USB_REQ_GET_DESCRIPTOR;
        phost->Control.setup.b.wValue.w       = USB_DESC_HID_REPORT;
        phost->Control.setup.b.wIndex.w       = itf;
        phost->Control.setup.b.wLength.w      = length;
    }

    return 0;//USBH_CtlReq(phost, buff, length);
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

    return 0;//USBH_CtlReq(phost, buff, length);
}

USBH_StatusTypeDef USBH_HID_SetIdle(USBH_HandleTypeDef *phost, uint8_t duration, uint8_t reportId, uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
                                         USB_REQ_TYPE_CLASS;


    phost->Control.setup.b.bRequest = USB_HID_SET_IDLE;
    phost->Control.setup.b.wValue.w = (uint16_t)(((uint32_t)duration << 8U) | (uint32_t)reportId);

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = 0U;

    return USBH_CtlReq(phost, 0U, 0U);
}


USBH_StatusTypeDef USBH_HID_SetReport(USBH_HandleTypeDef *phost,
                                      uint8_t reportType,
                                      uint8_t reportId,
                                      uint8_t *reportBuff,
                                      uint8_t reportLen, 
                                      uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS;


    phost->Control.setup.b.bRequest = USB_HID_SET_REPORT;
    phost->Control.setup.b.wValue.w = (uint16_t)(((uint32_t)reportType << 8U) | (uint32_t)reportId);

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = reportLen;

    return USBH_CtlReq(phost, reportBuff, (uint16_t)reportLen);
}


USBH_StatusTypeDef USBH_HID_GetReport(USBH_HandleTypeDef *phost,
                                      uint8_t reportType,
                                      uint8_t reportId,
                                      uint8_t *reportBuff,
                                      uint8_t reportLen,
                                      uint8_t itf)
{
    phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS;
    phost->Control.setup.b.bRequest = USB_HID_GET_REPORT;
    phost->Control.setup.b.wValue.w = (uint16_t)(((uint32_t)reportType << 8U) | (uint32_t)reportId);

    phost->Control.setup.b.wIndex.w = itf;
    phost->Control.setup.b.wLength.w = reportLen;

    return USBH_CtlReq(phost, reportBuff, (uint16_t)reportLen);
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

static void  USBH_HID_ParseHIDDesc(HID_DescTypeDef *desc, uint8_t *buf)
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
                desc->wItemLength = LE16((uint8_t *)pdesc + 7U);
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
    HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

    if ((phost->gState == HOST_CLASS_REQUEST) ||
        (phost->gState == HOST_INPUT) ||
        (phost->gState == HOST_SET_CONFIGURATION) ||
        (phost->gState == HOST_CHECK_CLASS) ||
        ((phost->gState == HOST_CLASS))) {

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
__weak void USBH_HID_EventCallback(USBH_HandleTypeDef *phost, uint8_t itf)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(phost);
    UNUSED(itf);
}