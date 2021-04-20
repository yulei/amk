/**
 * hhkbusb_matrix.c
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "generic_hal.h"
#include "report.h"
#include "matrix.h"
#include "host.h"
#include "timer.h"
#include "gpio_pin.h"
#include "usb_descriptors.h"
#include "amk_keymap.h"
#include "amk_printf.h"

#ifndef HHKB_MATRIX_DEBUG
#define HHKB_MATRIX_DEBUG 0
#endif

#if HHKB_MATRIX_DEBUG
#define hhkb_matrix_debug  amk_printf
#else
#define hhkb_matrix_debug(...)
#endif

extern UART_HandleTypeDef huart1;
pin_t reset_pin = RESET_PIN;

#define QUEUE_ITEM_SIZE   16                        // maximum size of the queue item
typedef struct {
    uint32_t    type;                               // type of the item
    uint8_t     data[QUEUE_ITEM_SIZE];
} report_item_t;

#define QUEUE_SIZE      64
typedef struct {
    report_item_t   items[QUEUE_SIZE];
    uint32_t        head;
    uint32_t        tail;
} report_queue_t;

static report_queue_t report_queue;
//static bool usb_suspended = false;


static bool report_queue_empty(report_queue_t* queue)
{
    return queue->head == queue->tail;
}

static bool report_queue_full(report_queue_t* queue)
{
    return ((queue->tail + 1) % QUEUE_SIZE) == queue->head;
}

static void report_queue_init(report_queue_t* queue)
{
    memset(&queue->items[0], 0, sizeof(queue->items));
    queue->head = queue->tail = 0;
}

static bool report_queue_put(report_queue_t* queue, report_item_t* item)
{
    if (report_queue_full(queue)) return false;

    queue->items[queue->tail] = *item;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    return true;
}

static bool report_queue_get(report_queue_t* queue, report_item_t* item)
{
    if (report_queue_empty(queue)) return false;

    *item = queue->items[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    return true;
}

// uart communication definitions
#define CONFIG_USER_START 0x0800FC00
#define CONFIG_USER_SIZE 0x00000400
#define CONFIG_JUMP_TO_APP_OFFSET 0x09
#define CMD_MAX_LEN 64
#define SYNC_BYTE_1 0xAA
#define SYNC_BYTE_2 0x55
#define RECV_DELAY 100

static void process_data(uint8_t d);
static void enqueue_command(uint8_t *cmd);
static void process_command(report_item_t *item);
static uint8_t compute_checksum(uint8_t *data, uint32_t size);
static void clear_jump_to_app(void);
static void send_set_leds(uint8_t led);

typedef enum
{
    CMD_KEY_REPORT,
    CMD_MOUSE_REPORT,
    CMD_SYSTEM_REPORT,
    CMD_CONSUMER_REPORT,
    CMD_RESET_TO_BOOTLOADER,
    CMD_SET_LEDS,
    CMD_KEYMAP_SET,
    CMD_KEYMAP_SET_ACK,
    CMD_KEYMAP_GET,
    CMD_KEYMAP_GET_ACK,
} command_t;

static uint8_t command_buf[CMD_MAX_LEN];
#define BLE_ROWS 5
#define BLE_COLS 14
typedef enum {
    BS_IDLE,
    BS_SEND,
} ble_state_t;

#define BLE_SYNC_TIMEOUT    1000
static uint16_t ble_keymaps[AMK_KEYMAP_MAX_LAYER][BLE_ROWS][BLE_COLS];
typedef struct {
    uint8_t     layer;
    uint8_t     row;
    uint8_t     col;
    uint8_t     finished;
    ble_state_t state;
    uint32_t    last;
} ble_sync_state_t;

static ble_sync_state_t ble_sync;
static void ble_sync_init(void);
static void ble_sync_process(void);
static void ble_sync_update(uint8_t layer, uint8_t row, uint8_t col, uint16_t key);


typedef enum
{
    USER_RESET, // user triggered the reset command
    PIN_RESET, // reset pin was pulled low
} reset_reason_t;

static void reset_to_bootloader(reset_reason_t reason)
{
    if ( reason == USER_RESET) {
        hhkb_matrix_debug("USER reset to bootloader\n");
    } else {
        hhkb_matrix_debug("PIN reset to bootloader\n");
    }
    clear_jump_to_app();

    __set_FAULTMASK(1);
    NVIC_SystemReset();

    // never return
    while (1)
        ;
}

static void process_data(uint8_t d)
{
    hhkb_matrix_debug("uart received: %d\n", d);
    static uint32_t count = 0;
    if (count == 0 && d != SYNC_BYTE_1) {
        hhkb_matrix_debug("SYNC BYTE 1: %x\n", d);
        return;
    } else if (count == 1 && d != SYNC_BYTE_2) {
        count = 0;
        hhkb_matrix_debug("SYNC BYTE 2: %x\n", d);
        return;
    }

    command_buf[count] = d;
    count++;
    if ((count > 2) && (count == (command_buf[2] + 2))) {
        // full packet received
        enqueue_command(&command_buf[2]);
        count = 0;
    }
}

static void enqueue_command(uint8_t *cmd)
{
    hhkb_matrix_debug("Enqueue Command: %d\n", cmd[2]);
    uint8_t checksum = compute_checksum(cmd + 2, cmd[0] - 2);
    if (checksum != cmd[1]) {
        // invalid checksum
        hhkb_matrix_debug("Checksum: LEN:%x, SRC:%x, CUR:%x\n", cmd[0], cmd[1], checksum);
        return;
    }

    report_item_t item;
    item.type = cmd[2];
    memcpy(&item.data[0], &cmd[3], cmd[0]-2);
    report_queue_put(&report_queue, &item);
}

static void process_command(report_item_t *item)
{
    switch (item->type) {
    case CMD_KEY_REPORT: {
        static report_keyboard_t report;
        hhkb_matrix_debug("Send key report\n");
        for (uint32_t i = 0; i < sizeof(report); i++) {
            hhkb_matrix_debug(" 0x%x", item->data[i]);
        }
        hhkb_matrix_debug("\n");

        memcpy(&report.raw[0], &item->data[0], sizeof(report));
        host_keyboard_send(&report);
    } break;
#ifdef MOUSE_ENABLE
    case CMD_MOUSE_REPORT: {
        static report_mouse_t report;
        hhkb_matrix_debug("Send mouse report\n");
        for (uint32_t i = 0; i < sizeof(report); i++) {
            hhkb_matrix_debug(" 0x%x", item->data[i]);
        }

        memcpy(&report, &item->data[0], sizeof(report));
        host_mouse_send(&report);
    } break;
#endif
#ifdef EXTRAKEY_ENABLE
    case CMD_SYSTEM_REPORT: {
        static uint16_t report;
        hhkb_matrix_debug("Send system report\n");
        for (uint32_t i = 0; i < sizeof(report); i++) {
            hhkb_matrix_debug(" 0x%x", item->data[i]);
        }
        memcpy(&report, &item->data[0], sizeof(report));
        host_system_send(report);

    } break;
    case CMD_CONSUMER_REPORT: {
        static uint16_t report;
        hhkb_matrix_debug("Send consumer report\n");
        for (uint32_t i = 0; i < sizeof(report); i++) {
            hhkb_matrix_debug(" 0x%x", item->data[i]);
        }
        memcpy(&report, &item->data[0], sizeof(report));
        host_consumer_send(report);
    } break;
#endif
    case CMD_RESET_TO_BOOTLOADER:
        reset_to_bootloader(USER_RESET);
        break;
    case CMD_KEYMAP_SET_ACK:
        // do nothing
        break;
    case CMD_KEYMAP_GET_ACK:
        ble_sync_update(item->data[0], item->data[1], item->data[2], (item->data[3]<<8) | item->data[4]);
        break;

    default:
        break;
    }
}

static uint8_t compute_checksum(uint8_t *data, uint32_t size)
{
    uint8_t checksum = 0;
    for (uint32_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum;
}

static void clear_jump_to_app(void)
{
    static uint8_t buf[CONFIG_USER_SIZE];
    uint32_t i = 0;
    uint16_t *pBuf = (uint16_t *)(&(buf[0]));
    uint16_t *pSrc = (uint16_t *)(CONFIG_USER_START);
    for (i = 0; i < CONFIG_USER_SIZE / 2; i++) {
        pBuf[i] = pSrc[i];
    }

    HAL_FLASH_Unlock();
    if (buf[CONFIG_JUMP_TO_APP_OFFSET] != 0) {
        buf[CONFIG_JUMP_TO_APP_OFFSET] = 0;
        FLASH_EraseInitTypeDef erase;
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.Banks = FLASH_BANK_1;
        erase.PageAddress = CONFIG_USER_START;
        erase.NbPages = 1;
        uint32_t error = 0;
        HAL_FLASHEx_Erase(&erase, &error);
    }

    for (i = 0; i < CONFIG_USER_SIZE / 2; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (uint32_t)(pSrc + i), pBuf[i]);
    }
    HAL_FLASH_Lock();
}

static void send_set_leds(uint8_t led)
{
    static uint8_t leds_cmd[8];
    leds_cmd[0] = SYNC_BYTE_1;
    leds_cmd[1] = SYNC_BYTE_2;
    leds_cmd[2] = 4;                // size
    leds_cmd[3] = CMD_SET_LEDS+led; // checksum
    leds_cmd[4] = CMD_SET_LEDS;     // command type
    leds_cmd[5] = led;              // led status

    HAL_UART_Transmit_DMA(&huart1, &leds_cmd[0], 6);
}

static matrix_row_t matrix[MATRIX_ROWS];
static uint8_t recv_char;



void matrix_init(void)
{
    report_queue_init(&report_queue);
    HAL_UART_Receive_IT(&huart1, &recv_char, 1);
    gpio_set_input_pullup(reset_pin);
    ble_sync_init();
}

uint8_t matrix_scan(void)
{
    if (gpio_read_pin(reset_pin) == 0) {
        reset_to_bootloader(PIN_RESET);
    }

    ble_sync_process();

    report_item_t item;
    while (report_queue_get(&report_queue, &item)) {
        process_command(&item);
    }
    return 1;
}

matrix_row_t matrix_get_row(uint8_t row) { return matrix[row]; }

void matrix_print(void)
{
    hhkb_matrix_debug("matrix_print\n");
}

void led_set(uint8_t usb_led)
{
    send_set_leds(usb_led);
}

void uart_recv_char(uint8_t c)
{
    process_data(c);
}

void uart_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    static uint8_t set_cmd[16];
    set_cmd[0] = SYNC_BYTE_1;
    set_cmd[1] = SYNC_BYTE_2;
    set_cmd[2] = 8;                 // size
    set_cmd[4] = CMD_KEYMAP_SET;    // command type
    set_cmd[5] = layer;             
    set_cmd[6] = row;
    set_cmd[7] = col;
    set_cmd[8] = ((keycode>>8)&0xFF);
    set_cmd[9] = (keycode&0xFF);

    set_cmd[3] = CMD_KEYMAP_SET+layer+row+col+set_cmd[8]+set_cmd[9]; // checksum

    HAL_UART_Transmit_DMA(&huart1, &set_cmd[0], 10);
}

void uart_keymap_get(uint8_t layer, uint8_t row, uint8_t col)
{
    static uint8_t get_cmd[16];
    get_cmd[0] = SYNC_BYTE_1;
    get_cmd[1] = SYNC_BYTE_2;
    get_cmd[2] = 6;                 // size
    get_cmd[4] = CMD_KEYMAP_GET;    // command type
    get_cmd[5] = layer;             
    get_cmd[6] = row;
    get_cmd[7] = col;

    get_cmd[3] = CMD_KEYMAP_GET+layer+row+col; // checksum

    HAL_UART_Transmit_DMA(&huart1, &get_cmd[0], 8);
}

void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    if (!((layer < AMK_KEYMAP_MAX_LAYER) && (row < BLE_ROWS) && (col < BLE_COLS))) return;

    ble_keymaps[layer][row][col] = keycode;
    uart_keymap_set(layer, row, col, keycode);
}

uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col)
{
    return ble_keymaps[layer][row][col];
}

static void ble_sync_init(void)
{
    for (uint8_t layer = 0; layer < AMK_KEYMAP_MAX_LAYER; layer++) {
        for (uint8_t row = 0; row < BLE_ROWS; row++) {
            for (uint8_t col = 0; col < BLE_COLS; col++) {
                ble_keymaps[layer][row][col] = 0;
            }
        }
    }
    ble_sync.layer      = 0;
    ble_sync.row        = 0;
    ble_sync.col        = 0;
#ifdef WEBCONFIG_ENABLE
    ble_sync.finished   = 0;
#else
    ble_sync.finished   = 1;
#endif

    ble_sync.state      = BS_IDLE;
    ble_sync.last       = timer_read32();
}

static void ble_sync_process(void)
{
    if (ble_sync.finished) return;

    // waiting for last command back
    if (ble_sync.state == BS_SEND) {
        if (timer_elapsed32(ble_sync.last) > BLE_SYNC_TIMEOUT) {
            hhkb_matrix_debug("BLESYNC: timer out, resent\n");
            ble_sync.state = BS_IDLE;
        } else {
            return;
        }
    }

    // get current keymap
    hhkb_matrix_debug("BLE SYNC: uart keymap get: layer:%d, row:%d, col:%d\n", ble_sync.layer, ble_sync.row, ble_sync.col);
    uart_keymap_get(ble_sync.layer, ble_sync.row, ble_sync.col);
    ble_sync.last = timer_read32();
    ble_sync.state = BS_SEND;
}

static void ble_sync_update(uint8_t layer, uint8_t row, uint8_t col, uint16_t key)
{
    hhkb_matrix_debug("BLESYNC: get keymap acked\n");
    if ((ble_sync.layer != layer) || (ble_sync.row != row) || (ble_sync.col != col)) {
        hhkb_matrix_debug("MISMATCH: layer:[%d]-[%d], row:[%d]-[%d], col:[%d]-[%d]\n", 
            ble_sync.layer, layer, ble_sync.row, row, ble_sync.col, col);
        ble_sync.state = BS_IDLE;
        return;
    }

    ble_keymaps[layer][row][col] = key;
    if (col == (BLE_COLS-1)) {
        if (row < (BLE_ROWS-1)) {
            col = 0;
            ble_sync.row++;
        } else {
            if (layer < (AMK_KEYMAP_MAX_LAYER-1)) {
                col = 0;
                row = 0;
                ble_sync.layer++;
            } else {
                // all done
                amk_printf("BLESYNC: finished\n");
                ble_sync.finished = 1;
            }
        }
    } else {
        ble_sync.col++;
    }

    hhkb_matrix_debug("BLESYNC: next key: layer:%d, row:%d, col:%d\n", ble_sync.layer, ble_sync.row, ble_sync.col);
    ble_sync.state = BS_IDLE;
}