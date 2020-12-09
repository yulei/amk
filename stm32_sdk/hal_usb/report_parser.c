/**
 * @file report_parser.c
 *
 */
#include <string.h>
#include "usbh_hid_def.h"
#include "report_parser.h"
#include "amk_printf.h"

// from qmk
#ifndef RAW_USAGE_PAGE
#    define RAW_USAGE_PAGE 0xFF60
#endif

#ifndef RAW_USAGE_ID
#    define RAW_USAGE_ID 0x61
#endif


static itf_report_desc_t descriptors[ITF_MAX];

static bool next_token(uint8_t* p, uint8_t* type, uint8_t* tag, uint8_t* size, uint32_t* value);

void rp_reset(void)
{
    memset(&descriptors[0], 0, sizeof(descriptors));
}

bool rp_parse(uint32_t itf, const void* data, uint32_t size)
{
    if (itf >= ITF_MAX) {
        amk_printf("invalid itf=%d\n", itf);
        return false;
    }

    uint8_t* p = (uint8_t*)data;
    uint8_t* end = p + size;
    itf_report_desc_t* itf_desc = &descriptors[itf];
    uint8_t index = 0;
    uint8_t collections = 0;
    uint8_t report_size = 0;
    uint8_t report_count = 0;
    uint32_t usage = 0;
    uint32_t usage_page = 0;
    while(p < end) {
        uint8_t type;
        uint8_t tag;
        uint8_t size;
        uint32_t value;
        if (next_token(p, &type, &tag, &size, &value)) {
            p += (1+size);
            switch (type) {
            case HID_ITEM_TYPE_MAIN:
                switch(tag){
                case HID_MAIN_ITEM_TAG_INPUT:
                    switch (itf_desc->desc[index].rd_type) {
                    case RDI_CONSUMER:
                        itf_desc->desc[index].consumer.keycode_size = (report_count*report_size)/8;
                        break;
                    case RDI_SYSTEM:
                        itf_desc->desc[index].system.keycode_size = (report_count*report_size)/8;
                        break;
                    case RDI_KEYBOARD: {
                        uint32_t ks = (report_count*report_size)/8;
                        if (usage_page == HID_USAGE_PAGE_KEYB) {
                            if (ks == 1) {
                                next_token(p, &type, &tag, &size, &value);
                                p += (1+size);
                                keyboard_descriptor_t* kbd = &itf_desc->desc[index].keyboard;
                                if (value==0x03) {
                                    // INPUT CONST
                                    kbd->orders[kbd->order++] = ORDER_RESERVED;
                                } else {
                                    kbd->orders[kbd->order++] = ORDER_MODIFIER;
                                }
                            } else {
                                keyboard_descriptor_t* kbd = &itf_desc->desc[index].keyboard;
                                kbd->orders[kbd->order++] = ORDER_KEYCODE;
                                kbd->keycode_size = ks;
                            }
                        }
                    }
                        break;
                    case RDI_MOUSE:
                        if (usage == 0x38 || usage == 0x0238) {
                            itf_desc->desc[index].mouse.wheel_enable = 1;
                        }
                        break;
                    case RDI_RAW:
                        itf_desc->desc[index].raw.input_size = (report_count*report_size)/8;
                        break;
                    default:
                        break;
                    }
                    break;
                case HID_MAIN_ITEM_TAG_OUTPUT:
                    if (itf_desc->desc[index].rd_type == RDI_KEYBOARD) {
                        keyboard_descriptor_t* kbd = &itf_desc->desc[index].keyboard;
                        kbd->orders[kbd->order++] = ORDER_LED;
                    } else if (itf_desc->desc[index].rd_type == RDI_RAW) {
                        itf_desc->desc[index].raw.output_size = (report_count*report_size)/8;
                    }
                    break;
                case HID_MAIN_ITEM_TAG_COLLECTION:
                    ++collections;
                    break;
                case HID_MAIN_ITEM_TAG_FEATURE:
                    break;
                case HID_MAIN_ITEM_TAG_ENDCOLLECTION:
                    --collections;
                    if (collections==0) {
                        index++;
                    }
                    break;
                default:
                    break;
                }
                break;
            case HID_ITEM_TYPE_GLOBAL:
                switch(tag) {
                case HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
                    usage_page = value;
                    if (collections == 0) {
                        // start new desc
                        if (value == HID_USAGE_PAGE_CONSUMER) {
                            itf_desc->desc[index].rd_type = RDI_CONSUMER;
                        } else if (value == HID_USAGE_PAGE_GEN_DES) {
                            next_token(p, &type, &tag, &size, &value);
                            p += (1+size);
                            if (value == HID_USAGE_KBD) {
                                itf_desc->desc[index].rd_type = RDI_KEYBOARD;
                            } else if (value == HID_USAGE_MOUSE) {
                                itf_desc->desc[index].rd_type = RDI_MOUSE;
                            } else if (value == HID_USAGE_SYSCTL) {
                                itf_desc->desc[index].rd_type = RDI_SYSTEM;
                            }
                        } else if (value == RAW_USAGE_PAGE) {
                            // raw hid mode
                            itf_desc->desc[index].rd_type = RDI_RAW;
                        } else {
                            // do not support
                            amk_printf("Unknown usage page: %x", usage_page);
                            return false;
                        }
                    }
                    break;
                case HID_GLOBAL_ITEM_TAG_LOG_MIN:
                    break;
                case HID_GLOBAL_ITEM_TAG_LOG_MAX:
                    break;
                case HID_GLOBAL_ITEM_TAG_PHY_MIN:
                    break;
                case HID_GLOBAL_ITEM_TAG_PHY_MAX:
                    break;
                case HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT:
                    break;
                case HID_GLOBAL_ITEM_TAG_UNIT:
                    break;
                case HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
                    report_size = value;
                    break;
                case HID_GLOBAL_ITEM_TAG_REPORT_ID:
                    itf_desc->desc[index].report_id = value;
                    break;
                case HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
                    report_count = value;
                    break;
                case HID_GLOBAL_ITEM_TAG_PUSH:
                    break;
                case HID_GLOBAL_ITEM_TAG_POP:
                    break;
                default:
                    break;
                }
                break;
            case HID_ITEM_TYPE_LOCAL:
                switch(tag) {
                case HID_LOCAL_ITEM_TAG_USAGE:
                    usage = value;
                    break;
                case HID_LOCAL_ITEM_TAG_USAGE_MIN:
                    break;
                case HID_LOCAL_ITEM_TAG_USAGE_MAX:
                    break;
                case HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX:
                    break;
                case HID_LOCAL_ITEM_TAG_DESIGNATOR_MIN:
                    break;
                case HID_LOCAL_ITEM_TAG_DESIGNATOR_MAX:
                    break;
                case HID_LOCAL_ITEM_TAG_STRING_INDEX:
                    break;
                case HID_LOCAL_ITEM_TAG_STRING_MIN:
                    break;
                case HID_LOCAL_ITEM_TAG_STRING_MAX:
                    break;
                case HID_LOCAL_ITEM_TAG_DELIMITER:
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }
    descriptors[itf].valid      = 1;
    descriptors[itf].itf        = itf;
    descriptors[itf].desc_count = index;
    return true;
}

itf_report_desc_t* rp_get(uint32_t itf)
{
    if (itf >= ITF_MAX) return NULL;
    return &descriptors[itf];
}

#define TOKEN_SIZE_MASK 0x03
#define TOKEN_SIZE_POS 0
#define TOKEN_TYPE_MASK 0x0C
#define TOKEN_TYPE_POS 2
#define TOKEN_TAG_MASK 0xF0
#define TOKEN_TAG_POS 4

#define TOKEN_DATA(x, mask, pos) (((x)&(mask))>>(pos))

bool next_token(uint8_t* p, uint8_t* type, uint8_t* tag, uint8_t* size, uint32_t* value)
{
    *size = TOKEN_DATA(*p, TOKEN_SIZE_MASK, TOKEN_SIZE_POS);
    *type = TOKEN_DATA(*p, TOKEN_TYPE_MASK, TOKEN_TYPE_POS);
    *tag = TOKEN_DATA(*p, TOKEN_TAG_MASK, TOKEN_TAG_POS);
    if (*size) {
        switch(*size) {
        case 1: {
            *value = p[1];
        } break;
        case 2: {
            *value = p[1] << 8 | p[2];
        } break;
        case 4: {
            *value = p[1] << 24 | p[2] << 16 | p[3] << 8 | p[4];
        } break;
        default:
            amk_printf("unknown token size:%d\n", *size);
            return false;
        }
    }

    return true;
}