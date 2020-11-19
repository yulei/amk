/**
 * @file usbh_hid_multi.h
 * 
 *  multiple hid interface handling
 */

#pragma once

#include "usbh_core.h"
#include "report_parser.h"

/* States for HID State Machine */
typedef enum {
    HID_INIT = 0,
    HID_IDLE,
    HID_SEND_DATA,
    HID_BUSY,
    HID_GET_DATA,
    HID_SYNC,
    HID_POLL,
    HID_ERROR,
} HID_StateTypeDef;

typedef enum {
    HID_REQ_INIT = 0,
    HID_REQ_IDLE,
    HID_REQ_GET_REPORT_DESC,
    HID_REQ_GET_HID_DESC,
    HID_REQ_SET_IDLE,
    HID_REQ_SET_PROTOCOL,
    HID_REQ_SET_REPORT,
} HID_CtlStateTypeDef;

typedef enum {
    HID_MOUSE     = 0x01,
    HID_KEYBOARD  = 0x02,
    HID_EXTRA     = 0x03,
    HID_RAW       = 0x04,
    HID_CONSOLE   = 0x05,
    HID_UNKNOWN   = 0xFF,
} HID_TypeTypeDef;

typedef struct {
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint16_t  bcdHID;
    uint8_t   bCountryCode;
    uint8_t   bNumDescriptors;
    uint8_t   bReportDescriptorType;
    uint16_t  wItemLength;
} HID_DescTypeDef;

typedef struct {
    uint8_t*  buf;
    uint16_t  head;
    uint16_t  tail;
    uint16_t  size;
    uint8_t   lock;
} FIFO_TypeDef;

typedef struct {
    uint8_t             OutPipe;
    uint8_t             InPipe;
    HID_StateTypeDef    state;
    uint8_t             OutEp;
    uint8_t             InEp;
    FIFO_TypeDef        fifo;
    uint8_t             *pData;
    uint16_t            length;
    uint16_t            poll;
    uint32_t            timer;
    uint8_t             DataReady;
    HID_DescTypeDef     HID_Desc;
    uint8_t             valid;
} HID_InterfaceTypeDef;


typedef USBH_StatusTypeDef(* init_t)(USBH_HandleTypeDef *phost);
typedef struct {
    HID_InterfaceTypeDef  itfs[ITF_MAX];
    HID_CtlStateTypeDef   ctl_state;
    uint8_t               ep_addr;
    init_t                Init;
} HID_HandleTypeDef;

// HID standard request
#define USB_HID_GET_REPORT                            0x01U
#define USB_HID_GET_IDLE                              0x02U
#define USB_HID_GET_PROTOCOL                          0x03U
#define USB_HID_SET_REPORT                            0x09U
#define USB_HID_SET_IDLE                              0x0AU
#define USB_HID_SET_PROTOCOL                          0x0BU

// HID class
#define USB_HID_CLASS                                 0x03U

// HID boot protocol
#define HID_BOOT_CODE                                 0x01U
#define HID_KEYBRD_BOOT_CODE                          0x01U
#define HID_MOUSE_BOOT_CODE                           0x02U

extern USBH_ClassTypeDef  HID_Multi_Class;
#define USBH_HID_CLASS    &HID_Multi_Class

USBH_StatusTypeDef USBH_HID_SetReport(USBH_HandleTypeDef *phost, uint8_t itf, uint8_t *reportBuff, uint8_t reportLen);
USBH_StatusTypeDef USBH_HID_GetReport(USBH_HandleTypeDef *phost, uint8_t itf, uint8_t *reportBuff, uint8_t reportLen);
USBH_StatusTypeDef USBH_HID_GetHIDReportDescriptor(USBH_HandleTypeDef *phost, uint8_t itf);
USBH_StatusTypeDef USBH_HID_GetHIDDescriptor(USBH_HandleTypeDef *phost, uint8_t itf);
USBH_StatusTypeDef USBH_HID_SetIdle(USBH_HandleTypeDef *phost, uint8_t itf, uint8_t duration, uint8_t reportId);
USBH_StatusTypeDef USBH_HID_SetProtocol(USBH_HandleTypeDef *phost, uint8_t itf, uint8_t protocol); 
uint8_t USBH_HID_GetPollInterval(USBH_HandleTypeDef *phost, uint8_t itf);

HID_TypeTypeDef USBH_HID_GetDeviceType(USBH_HandleTypeDef *phost, uint8_t itf);

void USBH_HID_FifoInit(FIFO_TypeDef *f, uint8_t *buf, uint16_t size);
uint16_t  USBH_HID_FifoRead(FIFO_TypeDef *f, void *buf, uint16_t  nbytes);
uint16_t  USBH_HID_FifoWrite(FIFO_TypeDef *f, void *buf, uint16_t nbytes);

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost, uint8_t itf);