/** 
 * @file usb_descriptors.c 
 */

#include "usb_descriptors.h"
#include "usb_desc_def.h"
#include "tusb.h"

#define TUD_COMPOSITE_DEVICE_DESCRIPTOR(usb_ver, vender_id, product_id, device_ver, str_manufacture, str_product, str_serial ) \
    18, TUSB_DESC_DEVICE, U16_TO_U8S_LE(usb_ver), \
    0x00, 0x00, 0x00, CFG_TUD_ENDPOINT0_SIZE, \
    U16_TO_U8S_LE(vender_id), U16_TO_U8S_LE(product_id), U16_TO_U8S_LE(device_ver), \
    str_manufacture, str_product, str_serial, 1

// Extra key report
#define TUD_HID_REPORT_DESC_EXTRA(system, consumer) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )         ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_SYSTEM_CONTROL )         ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION       )         ,\
    HID_REPORT_ITEM( system, 8, RI_TYPE_GLOBAL, 1     )         ,\
    HID_LOGICAL_MIN_N( 0x0001, 2                           )    ,\
    HID_LOGICAL_MAX_N( 0x00B7, 2                           )    ,\
    /* System Power Down */ \
    HID_USAGE_MIN_N  ( 0x0001, 2                           )    ,\
    /* System Display LCD Autoscale */ \
    HID_USAGE_MAX_N  ( 0x00B7, 2                           )    ,\
    HID_REPORT_COUNT ( 1                                   )    ,\
    HID_REPORT_SIZE  ( 16                                  )    ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )    ,\
    HID_COLLECTION_END, \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER    )               ,\
    HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL )               ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION )               ,\
    HID_REPORT_ITEM( consumer, 8, RI_TYPE_GLOBAL, 1        )    ,\
    HID_LOGICAL_MIN_N( 0x0001, 2                           )    ,\
    HID_LOGICAL_MAX_N( 0x029C, 2                           )    ,\
    HID_USAGE_MIN_N  ( 0x0001, 2                           )    ,\
    HID_USAGE_MAX_N  ( 0x029C, 2                           )    ,\
    HID_REPORT_COUNT ( 1                                   )    ,\
    HID_REPORT_SIZE  ( 16                                  )    ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )    ,\
    HID_COLLECTION_END \

#define TUD_HID_REPORT_DESC_NKRO(...) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )         ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD       )         ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION       )         ,\
    /* Report ID if any */ \
    __VA_ARGS__ \
    /* 8 bits Modifier Keys (Shfit, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                       ,\
        HID_USAGE_MIN    ( 224                                    )  ,\
        HID_USAGE_MAX    ( 231                                    )  ,\
        HID_LOGICAL_MIN  ( 0                                      )  ,\
        HID_LOGICAL_MAX  ( 1                                      )  ,\
        HID_REPORT_COUNT ( 8                                      )  ,\
        HID_REPORT_SIZE  ( 1                                      )  ,\
        HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD ) ,\
        HID_USAGE_MIN    ( 0                                   )  ,\
        HID_USAGE_MAX    ( (AMK_NKRO_TOTAL_SIZE-2)*8             )  ,\
        HID_LOGICAL_MIN  ( 0                                   )  ,\
        HID_LOGICAL_MAX  ( 1                                   )  ,\
        HID_REPORT_COUNT ( (AMK_NKRO_TOTAL_SIZE-2)*8             )  ,\
        HID_REPORT_SIZE  ( 1                                   )  ,\
        HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )  ,\
    /* 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED ) ,\
        HID_USAGE_MIN    ( 1                                       ) ,\
        HID_USAGE_MAX    ( 5                                       ) ,\
        HID_REPORT_COUNT ( 5                                       ) ,\
        HID_REPORT_SIZE  ( 1                                       ) ,\
        HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
        /* led padding */ \
        HID_REPORT_COUNT ( 1                                       ) ,\
        HID_REPORT_SIZE  ( 3                                       ) ,\
        HID_OUTPUT       ( HID_CONSTANT                            ) ,\
    HID_COLLECTION_END \

#define VIAL_USAGE_PAGE     0xFF60
#define VIAL_USAGE_ID       0x61
#define VIAL_EPSIZE         CFG_TUD_HID_EP_BUFSIZE

#define TUD_HID_REPORT_DESC_VIAL(report_size, ...) \
    HID_USAGE_PAGE_N ( VIAL_USAGE_PAGE, 2                                           ),\
    HID_USAGE        ( VIAL_USAGE_ID                                                ),\
    HID_COLLECTION   ( HID_COLLECTION_APPLICATION                                   ),\
        /* Report ID if any */\
        __VA_ARGS__ \
        /* Input */ \
        HID_USAGE       ( 0x62                                                      ),\
        HID_LOGICAL_MIN ( 0x00                                                      ),\
        HID_LOGICAL_MAX ( 0xff                                                      ),\
        HID_REPORT_SIZE ( 8                                                         ),\
        HID_REPORT_COUNT( report_size                                               ),\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE                    ),\
        /* Output */ \
        HID_USAGE       ( 0x63                                                      ),\
        HID_LOGICAL_MIN ( 0x00                                                      ),\
        HID_LOGICAL_MAX ( 0xff                                                      ),\
        HID_REPORT_SIZE ( 8                                                         ),\
        HID_REPORT_COUNT( report_size                                               ),\
        HID_OUTPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_NON_VOLATILE ),\
    HID_COLLECTION_END \

// audio descriptior
// AUDIO simple descriptor (UAC2) for mono speaker
// - 1 Input Terminal, 2 Feature Unit (Mute and Volume Control), 3 Output Terminal, 4 Clock Source

#define TUD_AUDIO_MONO_SPEAKER_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN\
  + TUD_AUDIO_DESC_STD_AC_LEN\
  + TUD_AUDIO_DESC_CS_AC_LEN\
  + TUD_AUDIO_DESC_CLK_SRC_LEN\
  + TUD_AUDIO_DESC_INPUT_TERM_LEN\
  + TUD_AUDIO_DESC_OUTPUT_TERM_LEN\
  + TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN\
  + TUD_AUDIO_DESC_STD_AS_INT_LEN\
  + TUD_AUDIO_DESC_STD_AS_INT_LEN\
  + TUD_AUDIO_DESC_CS_AS_INT_LEN\
  + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
  + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
  + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN\
  + TUD_AUDIO_DESC_STD_AS_ISO_FB_EP_LEN)

#define TUD_AUDIO_DESC_IAD_UDF(_firstitfs, _nitfs, _stridx) \
  TUD_AUDIO_DESC_IAD_LEN, TUSB_DESC_INTERFACE_ASSOCIATION, _firstitfs, _nitfs, TUSB_CLASS_AUDIO, AUDIO_FUNCTION_SUBCLASS_UNDEFINED, 0x20, _stridx
#define TUD_AUDIO_DESC_STD_AC_UDF(_itfnum, _nEPs, _stridx) /* _nEPs is 0 or 1 */\
  TUD_AUDIO_DESC_STD_AC_LEN, TUSB_DESC_INTERFACE, _itfnum, /* fixed to zero */ 0x00, _nEPs, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, 0x20, _stridx
#define TUD_AUDIO_DESC_STD_AS_INT_UDF(_itfnum, _altset, _nEPs, _stridx) \
  TUD_AUDIO_DESC_STD_AS_INT_LEN, TUSB_DESC_INTERFACE, _itfnum, _altset, _nEPs, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, 0x20, _stridx

#define TUD_AUDIO_MONO_SPEAKER_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epout, _epsize, _epfb) \
  /* Standard Interface Association Descriptor (IAD) */\
  TUD_AUDIO_DESC_IAD_UDF(/*_firstitfs*/ _itfnum, /*_nitfs*/ 0x02, /*_stridx*/ 0x00),\
  /* Standard AC Interface Descriptor(4.7.1) */\
  TUD_AUDIO_DESC_STD_AC_UDF(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx),\
  /* Class-Specific AC Interface Header Descriptor(4.7.2) */\
  TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_DESKTOP_SPEAKER, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN+TUD_AUDIO_DESC_INPUT_TERM_LEN+TUD_AUDIO_DESC_OUTPUT_TERM_LEN+TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS),\
  /* Clock Source Descriptor(4.7.2.1) */\
  TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ 0x04, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ 0x01,  /*_stridx*/ 0x00),\
  /* Input Terminal Descriptor(4.7.2.4) */\
  TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ 0x01, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_clkid*/ 0x04, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00),\
  /* Output Terminal Descriptor(4.7.2.5) */\
  TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ 0x03, /*_termtype*/ AUDIO_TERM_TYPE_OUT_DESKTOP_SPEAKER, /*_assocTerm*/ 0x01, /*_srcid*/ 0x02, /*_clkid*/ 0x04, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00),\
  /* Feature Unit Descriptor(4.7.2.8) */\
  TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL(/*_unitid*/ 0x02, /*_srcid*/ 0x01, /*_ctrlch0master*/ 0 * (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS), /*_ctrlch1*/ 0 * (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS), /*_stridx*/ 0x00),\
  /* Standard AS Interface Descriptor(4.9.1) */\
  /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */\
  TUD_AUDIO_DESC_STD_AS_INT_UDF(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00),\
  /* Standard AS Interface Descriptor(4.9.1) */\
  /* Interface 1, Alternate 1 - alternate interface for data streaming */\
  TUD_AUDIO_DESC_STD_AS_INT_UDF(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x02, /*_stridx*/ 0x00),\
  /* Class-Specific AS Interface Descriptor(4.9.2) */\
  TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ 0x01, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
  /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
  TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample),\
  /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
  TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout, /*_attr*/ (TUSB_XFER_ISOCHRONOUS | TUSB_ISO_EP_ATT_ASYNCHRONOUS | TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ TUD_OPT_HIGH_SPEED ? 0x04 : 0x01),\
  /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
  TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000),\
  /* Standard AS Isochronous Feedback Endpoint Descriptor(4.10.2.1) */\
  TUD_AUDIO_DESC_STD_AS_ISO_FB_EP(/*_ep*/ _epfb, /*_interval*/ 1)\

// Device Descriptors
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
#if defined(AUDIO_ENABLE) || defined(CDC_ENABLE)
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
#else
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
#endif
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = VENDOR_ID,
    .idProduct          = PRODUCT_ID,
    .bcdDevice          = DEVICE_VER,

    .iManufacturer      = DESC_STR_MANUFACTURE,
    .iProduct           = DESC_STR_PRODUCT,
    .iSerialNumber      = DESC_STR_SERIAL,

    .bNumConfigurations = 0x01
};

// Device Descriptors
tusb_desc_device_t const desc_device_dyn =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = VENDOR_ID,
    .idProduct          = (uint16_t)(~PRODUCT_ID),
    .bcdDevice          = DEVICE_VER,

    .iManufacturer      = DESC_STR_MANUFACTURE,
    .iProduct           = DESC_STR_PRODUCT,
    .iSerialNumber      = DESC_STR_SERIAL,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
uint8_t const* tud_descriptor_device_cb(void)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return (uint8_t*) &desc_device_dyn;
    }
#endif
    return (uint8_t *) &desc_device;
}

uint32_t tud_descriptor_device_size(void)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return sizeof(desc_device_dyn);
    }
#endif
    return sizeof(desc_device);
}

// HID Keyboard Report Descriptor
static uint8_t desc_hid_report_kbd[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD(),
};

// HID other report
static uint8_t desc_hid_report_other[] =
{
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(HID_REPORT_ID_MOUSE) ),
    TUD_HID_REPORT_DESC_EXTRA( HID_REPORT_ID_SYSTEM, HID_REPORT_ID_CONSUMER ),
#ifdef NKRO_AUTO_ENABLE
    TUD_HID_REPORT_DESC_NKRO( HID_REPORT_ID(HID_REPORT_ID_NKRO) )          
#endif
};

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const* tud_descriptor_hid_report_kbd_cb(void)
{
    return desc_hid_report_kbd;
}

uint32_t tud_descriptor_hid_report_kbd_size(void)
{
    return sizeof(desc_hid_report_kbd);
}

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const* tud_descriptor_hid_report_other_cb(void)
{
    return desc_hid_report_other;
}

uint32_t tud_descriptor_hid_report_other_size(void)
{
    return sizeof(desc_hid_report_other);
}

#ifdef VIAL_ENABLE
static uint8_t desc_hid_report_vial[] =
{
    TUD_HID_REPORT_DESC_VIAL(VIAL_EPSIZE),
};

uint8_t const* tud_descriptor_hid_report_vial_cb(void)
{
    return desc_hid_report_vial;
}

uint32_t tud_descriptor_hid_report_vial_size(void)
{
    return sizeof(desc_hid_report_vial);
}
#endif

// Configuration Descriptor
#ifdef HID_OTHER_ENABLE
#define HID_OTHER_DESC_LEN TUD_HID_DESC_LEN 
#else
#define HID_OTHER_DESC_LEN 0 
#endif

#ifdef VIAL_ENABLE
#define VIAL_DESC_LEN TUD_HID_INOUT_DESC_LEN 
#else
#define VIAL_DESC_LEN 0 
#endif

#ifdef CDC_ENABLE
#define CDC_DESC_LEN TUD_CDC_DESC_LEN 
#else
#define CDC_DESC_LEN 0 
#endif

#ifdef MSC_ENABLE 
#define MSCUSB_DESC_LEN TUD_MSC_DESC_LEN 
#else 
#define MSCUSB_DESC_LEN 0 
#endif

#ifdef AUDIO_ENABLE
#define AUDIOUSB_DESC_LEN   TUD_AUDIO_MONO_SPEAKER_DESC_LEN 
#else
#define AUDIOUSB_DESC_LEN   0
#endif

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + HID_OTHER_DESC_LEN + VIAL_DESC_LEN + MSCUSB_DESC_LEN + AUDIOUSB_DESC_LEN + CDC_DESC_LEN)

#define CONFIG_LEN_MSC (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + MSCUSB_DESC_LEN)

#define CONFIG_LEN_VIAL (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + VIAL_DESC_LEN)

#define CONFIG_LEN_AUDIO (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + AUDIOUSB_DESC_LEN)

static uint8_t desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),

#ifdef HID_OTHER_ENABLE
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
#endif

#ifdef CDC_ENABLE
  // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
  TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_DATAOUT, 0x80 | EPNUM_CDC_DATAIN, 64),
#endif

#ifdef VIAL_ENABLE
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, CFG_TUD_HID_POLL_INTERVAL),
#endif

#ifdef MSC_ENABLE
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, 0x80|EPNUM_MSC_IN, CFG_TUD_MSC_EPSIZE),
#endif

#ifdef AUDIO_ENABLE
    TUD_AUDIO_MONO_SPEAKER_DESCRIPTOR(ITF_NUM_AUDIO, 0, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_BITS_PER_SAMPLE_RX, EPNUM_AUDIO_OUT, CFG_TUD_AUDIO_EP_SZ_OUT, 0x80|EPNUM_AUDIO_IN),
#endif

};

#ifdef DYNAMIC_CONFIGURATION

#ifdef MSC_ENABLE
#define ITF_NUM_MSC_TOTAL  3
#else
#define ITF_NUM_MSC_TOTAL  2
#endif
static uint8_t desc_with_msc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_MSC_TOTAL, 0, CONFIG_LEN_MSC, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    // Interface number, string index, EP Out & EP In address, EP size
#ifdef MSC_ENABLE
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, 0x80|EPNUM_MSC_IN, CFG_TUD_MSC_EPSIZE),
#endif
};

#ifdef VIAL_ENABLE
#define ITF_NUM_VIAL_TOTAL  3
#else
#define ITF_NUM_VIAL_TOTAL  2
#endif
static uint8_t desc_with_vial[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_VIAL_TOTAL, 0, CONFIG_LEN_VIAL, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
#ifdef VIAL_ENABLE
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, CFG_TUD_HID_POLL_INTERVAL),
#endif
};

#ifdef AUDIO_ENABLE
#define ITF_NUM_AUDIO_TOTAL  4
#else
#define ITF_NUM_AUDIO_TOTAL  2
#endif
static uint8_t desc_with_audio[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_AUDIO_TOTAL, 0, CONFIG_LEN_AUDIO, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
#ifdef AUDIO_ENABLE
    TUD_AUDIO_MONO_SPEAKER_DESCRIPTOR(ITF_NUM_AUDIO, 0, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_BITS_PER_SAMPLE_RX, EPNUM_AUDIO_OUT, CFG_TUD_AUDIO_EP_SZ_OUT, 0x80|EPNUM_AUDIO_IN),
#endif
};

#endif

// Invoked when received GET CONFIGURATION DESCRIPTOR
uint8_t const* tud_descriptor_configuration_cb(uint8_t index)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return desc_with_msc;
    }

    if (usb_setting & USB_AUDIO_BIT) {
        return desc_with_audio;
    }

    //if (usb_setting & USB_VIAL_BIT) {
        return desc_with_vial;
    //}

#endif

    return desc_configuration;
}

uint32_t tud_descriptor_configuration_size(uint8_t index)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return sizeof(desc_with_msc);
    }

    if (usb_setting & USB_AUDIO_BIT) {
        return sizeof(desc_with_audio);
    }

    //if (usb_setting & USB_VIAL_BIT) {
        return sizeof(desc_with_vial);
    //}

#endif
    return sizeof(desc_configuration);
}

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_report_kbd_cb();
    } 
    #ifdef HID_OTHER_ENABLE
    else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_report_other_cb();
    }
    #endif
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_report_vial_cb();
    }
    #endif
    return NULL;
}

uint32_t tud_hid_descriptor_report_size(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_report_kbd_size();
    } 
    #ifdef HID_OTHER_ENABLE
    else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_report_other_size();
    }
    #endif
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_report_vial_size();
    }
    #endif
    return 0;
}

static uint8_t desc_hid_kbd[] = {
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
};


uint8_t const* tud_hid_descriptor_interface_cb(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_interface_kbd_cb();
    }
    #ifdef HID_OTHER_ENABLE
    else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_interface_other_cb();
    }
    #endif
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_interface_vial_cb();
    }
    #endif
    return NULL;
}

uint32_t tud_hid_descriptor_interface_size(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_interface_kbd_size();
    } 
    #ifdef HID_OTHER_ENABLE
    else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_interface_other_size();
    }
    #endif
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_interface_vial_size();
    }
    #endif
    return 0;
}

// Invoded when received GET HID DESCRIPTOR
uint8_t const* tud_descriptor_hid_interface_kbd_cb(void)
{
    return desc_hid_kbd;
}

uint32_t tud_descriptor_hid_interface_kbd_size(void)
{
    return sizeof(desc_hid_kbd);
}

#ifdef HID_OTHER_ENABLE
static uint8_t desc_hid_other[] = {
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
};

// Invoded when received GET HID DESCRIPTOR
uint8_t const* tud_descriptor_hid_interface_other_cb(void)
{
    return desc_hid_other;
}

uint32_t tud_descriptor_hid_interface_other_size(void)
{
    return sizeof(desc_hid_other);
}
#endif

#ifdef VIAL_ENABLE
static uint8_t desc_hid_vial[] = {
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, CFG_TUD_HID_POLL_INTERVAL),
};

uint8_t const* tud_descriptor_hid_interface_vial_cb(void)
{
    return desc_hid_vial;
}

uint32_t tud_descriptor_hid_interface_vial_size(void)
{
    return sizeof(desc_hid_vial);
}

#endif

// String Descriptors

// array of pointer to string descriptors
char const* string_desc_arr [] =
{
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    TU_XSTRING(MANUFACTURER),       // 1: Manufacturer
    TU_XSTRING(PRODUCT),            // 2: Product
#ifdef VIAL_ENABLE
    "vial:f64c2b3c",                // 3: Serials, should use chip ID
#else
    "amk:4d58",                     // 3: Serials, should use chip ID
#endif
    "Configuration",                // 4: Device configuration 
    "HID Keyboard",                 // 5: Hid keyboard
    "HID Extra",                    // 6: Hid extra key
};

static uint16_t _desc_str[32];

//static uint16_t *get_device_serial(uint16_t* length);
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint16_t length = 0;
    return (uint16_t *)get_descriptor_str(index, &length);
}

// Invoked when received GET STRING DESCRIPTOR request
uint8_t *get_descriptor_str(uint8_t index, uint16_t *length)
{
    uint8_t chr_count;

    if ( index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } /*else if (index == DESC_STR_SERIAL) {
        return (uint8_t*)get_device_serial(length);
    }*/ else {
        // Convert ASCII string into UTF-16

        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if ( chr_count > 31 ) chr_count = 31;

        for(uint8_t i=0; i<chr_count; i++) {
          _desc_str[1+i] = str[i];
        }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
  *length = _desc_str[0] & 0xFF;

  return (uint8_t*)_desc_str;
}

#if 0
static uint8_t _desc_serial[64];

static void to_unicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
    uint8_t idx = 0;

    for (idx = 0; idx < len; idx++) {
        if (((value >> 28)) < 0xA) {
            pbuf[2 * idx] = (value >> 28) + '0';
        } else {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }
        value = value << 4;
        pbuf[2 * idx + 1] = 0;
    }
}

#if defined(NRF52840_XXAA) || defined(NRF52)
#include "nrf.h"
#else
#include "generic_hal.h"
#endif
uint16_t* get_device_serial(uint16_t* length)
{
  uint32_t d0, d1;

#if defined(NRF52840_XXAA) || defined(NRF52)
    d0 = NRF_FICR->DEVICEADDR[0];
    d1 = NRF_FICR->DEVICEADDR[1] | 0xC000;
#else
    d0  = *(uint32_t *)(UID_BASE) + *(uint32_t *)(UID_BASE + 0x8);
    d1  = *(uint32_t *)(UID_BASE + 0x4);
#endif

    memset(&_desc_serial[0], 0, sizeof(_desc_serial));
    to_unicode(d0, &_desc_serial[2], 8);
    to_unicode(d1, &_desc_serial[18], 4);
    _desc_serial[0] = TUSB_DESC_STRING;
    _desc_serial[1] = 2 * 16 + 2;
    *length = _desc_serial[1];
    return (uint16_t *)_desc_serial;
}
#endif