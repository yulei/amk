/**
 * @file report_parser.c
 *
 */
#include "report_parser.h"
#include "usbh_hid_usage.h"
#include "usbh_hid.h"
//
// there will have 4 interfaces at maximum
//
// keybaord, mouse, extra(system & consumer), nkro
// in tmk: they will be list in order if the specific feature was opened
// in qmk: mouse&extra&nkro will share the same interface, and keyboard 
// interface will also share with them if KEYBOARD_SHARE_EP was enabled

// report descriptor item type
enum {
    RDI_NONE,
    RDI_KEYBOARD,
    RDI_MOUSE,
    RDI_SYSTEM,
    RDI_CONSUMER,
    RDI_NKRO,
};

#define KEYCODE_LEN_MAX    6            // boot keyboard use 1 byte for modified, 1 byte reserved, 6 bytes for keycodes, 1 byte for led 
typedef struct {
    uint8_t interface;
    uint8_t report_id;
    uint8_t modifier;                   // shift/ctrl/gui/alt
    uint8_t reserved;                   // reserved 
    uint8_t keycodes[KEYCODE_LEN_MAX];  // maximum length of the keycodes
    uint8_t led;                        // led 
} keyboard_descriptor_t;

// boot mouse use 1 byte for button, 2 bytes for x/y, 2 bytes for wheel h/v
typedef struct {
    uint8_t interface;
    uint8_t report_id;
    uint8_t button;
    int8_t x;
    int8_t y;
    int8_t v;
    int8_t h;
} mouse_descriptor_t;

#define SYSTEM_LEN_MAX      2
typedef struct {
    uint8_t interface;
    uint8_t report_id;
    uint16_t keycode;
} system_descriptor_t;

#define CONSUMER_LEN_MAX    2
typedef struct {
    uint8_t interface;
    uint8_t report_id;
    uint16_t keycode;
} consumer_descriptor_t;

#define NKRO_LEN_MAX        32          // tmk has 31, qmk has 30
enum {
    ORDER_MODIFIER,
    ORDER_LED,
    ORDER_KEYCODE,
    ORDER_MAX,
};

typedef struct {
    uint8_t interface;
    uint8_t report_id;
    uint8_t modifier;
    uint8_t led;
    uint8_t keycode_size;
    uint8_t keycode_data[NKRO_LEN_MAX];
    uint8_t orders[ORDER_MAX];          // order of the modifier, led and keycodes
} nkro_descriptor_t;

typedef struct {
    union {
        keyboard_descriptor_t keyboard;
        mouse_descriptor_t mouse;
        system_descriptor_t system;
        consumer_descriptor_t consumer;
        nkro_descriptor_t nkro;
    };
    uint8_t rd_type;
} report_descriptor_t;

#define ITF_MAX 4
static report_descriptor_t descriptors[ITF_MAX];
static uint8_t avail_descs = 0;

static uint32_t pase_desktop(report_descriptor_t* desc, uint8_t* p);
static uint32_t pase_consumer(report_descriptor_t* desc, uint8_t* p);

bool parse_report_descriptor(uint32_t itf, const void* data, uint32_t size)
{
    uint8_t* p = (uint8_t*)data;
    uint8_t* end = p + size;
    uint32_t parsed;
    while(p < end) {
        switch(*p){
        case HID_USAGE_PAGE_GEN_DES:
            parsed = parse_desktop(&descriptors[avail_descs], p);
        break;
        case HID_USAGE_PAGE_CONSUMER:
            parsed = parse_consumer(&descriptors[avail_descs], p);
        break;
        default:
            return false; // unsupport usage page
        }
        if (parsed == 0) {
            // some error occured
            return false;
        } else {
            p += parsed;
            avail_descs++;
        }
    }
    return true;
}

void repport_decode(uint32_t itf, const void* data, uint32_t size)
{}