/**
 * @file usbh_hid_multi.h
 * 
 *  multiple hid interface handling
 */

#pragma once

#include "usbh_hid_def.h"
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
    HID_UNKNOWN = 0,
    HID_MOUSE,
    HID_KEYBOARD,
    HID_EXTRA,
    HID_RAW,
    HID_CONSOLE,
} HID_TypeTypeDef;

typedef struct {
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint16_t  bcdHID;
    uint8_t   bCountryCode;
    uint8_t   bNumDescriptors;
    uint8_t   bReportDescriptorType;
    uint16_t  wDescriptorLength;
} HID_DescTypeDef;

typedef struct {
    uint8_t*  buf;
    uint16_t  head;
    uint16_t  tail;
    uint16_t  size;
    uint8_t   lock;
} FIFO_TypeDef;

typedef struct {
    uint8_t             out_pipe;
    uint8_t             in_pipe;
    HID_StateTypeDef    state;
    HID_CtlStateTypeDef ctl_state;
    uint8_t             out_ep;
    uint8_t             in_ep;
    FIFO_TypeDef        fifo;
    uint8_t             fifo_buf[HID_QUEUE_SIZE*HID_PACKET_SIZE];
    uint8_t             report_buf[HID_REPORT_BUF_SIZE];
    uint8_t             report_size;
    uint16_t            packet_size;
    uint16_t            poll;
    uint32_t            timer;
    uint8_t             data_ready;
    HID_DescTypeDef     hid_desc;
    uint8_t             *report_desc;
    uint8_t             valid;
    HID_TypeTypeDef     type;
} HID_InterfaceTypeDef;


typedef USBH_StatusTypeDef(* init_t)(USBH_HandleTypeDef *phost);
typedef struct {
    HID_InterfaceTypeDef    itfs[ITF_MAX];
    uint8_t                 itf_num;
    uint8_t                 inited;
    init_t                  Init;
    uint8_t                 current;
    void                    *host;
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

USBH_StatusTypeDef USBH_HID_SetReport(USBH_HandleTypeDef *phost, uint8_t type, uint8_t id, uint8_t *reportBuff, uint8_t reportLen, uint8_t itf);
USBH_StatusTypeDef USBH_HID_GetReport(USBH_HandleTypeDef *phost, uint8_t type, uint8_t id, uint8_t *reportBuff, uint8_t reportLen, uint8_t itf);
USBH_StatusTypeDef USBH_HID_GetHIDReportDescriptor(USBH_HandleTypeDef *phost, uint16_t length, uint8_t itf);
USBH_StatusTypeDef USBH_HID_GetHIDDescriptor(USBH_HandleTypeDef *phost, uint16_t length, uint8_t itf);
USBH_StatusTypeDef USBH_HID_SetIdle(USBH_HandleTypeDef *phost, uint8_t duration, uint8_t reportId, uint8_t it);
USBH_StatusTypeDef USBH_HID_SetProtocol(USBH_HandleTypeDef *phost, uint8_t protocol, uint8_t it); 
uint8_t USBH_HID_GetPollInterval(USBH_HandleTypeDef *phost, uint8_t itf);
HID_TypeTypeDef USBH_HID_GetDeviceType(USBH_HandleTypeDef *phost, uint8_t itf);

void USBH_HID_FifoInit(FIFO_TypeDef *f, uint8_t *buf, uint16_t size);
uint16_t USBH_HID_FifoRead(FIFO_TypeDef *f, void *buf, uint16_t  nbytes);
uint16_t USBH_HID_FifoWrite(FIFO_TypeDef *f, void *buf, uint16_t nbytes);

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost);