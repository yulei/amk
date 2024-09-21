/**
 * @file rf_driver.c
 */

#include <stdbool.h>
#include <string.h>
#include "rf_driver.h"
#include "amk_printf.h"
#include "amk_gpio.h"
#include "amk_ring_buffer.h"
#include "usb_descriptors.h"
#include "wait.h"

#ifndef RF_DRIVER_DEBUG
#define RF_DRIVER_DEBUG 1
#endif

#if RF_DRIVER_DEBUG
#define rf_driver_debug  amk_printf
#else
#define rf_driver_debug(...)
#endif

extern UART_HandleTypeDef huart_rf;


#define QUEUE_ITEM_SIZE   16                        // maximum size of the queue item
typedef struct {
    uint16_t    type;                               // type of the item
    uint16_t    size;                               // type of the item
    uint8_t     data[QUEUE_ITEM_SIZE];
} report_item_t;

#define QUEUE_SIZE      64
typedef struct {
    report_item_t   items[QUEUE_SIZE];
    uint32_t        head;
    uint32_t        tail;
} report_queue_t;

static report_queue_t report_queue;

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

static void process_data(uint8_t d);
static void enqueue_command(uint8_t *cmd);
static void process_report(report_item_t *item);
static uint8_t compute_checksum(uint8_t *data, uint32_t size);
static void rf_cmd_set_leds(uint8_t led);

static uint8_t recv_buf[CMD_MAX_LEN];
static uint32_t recv_buf_count = 0;

static uint8_t ring_buffer_data[RING_BUF_MAX_LEN];
static ring_buffer_t ring_buffer;
static uint8_t send_buf[CMD_MAX_LEN];

static void process_data(uint8_t d)
{
    rf_driver_debug("uart received: %d, current count=%d\n", d, recv_buf_count);
    if (recv_buf_count == 0 && d != SYNC_BYTE_1) {
        rf_driver_debug("SYNC BYTE 1: %x\n", d);
        return;
    } else if (recv_buf_count == 1 && d != SYNC_BYTE_2) {
        recv_buf_count = 0;
        memset(recv_buf, 0, sizeof(recv_buf));
        rf_driver_debug("SYNC BYTE 2: %x\n", d);
        return;
    }

    if (recv_buf_count >= CMD_MAX_LEN) {
        rf_driver_debug("UART command oversize\n");
        memset(recv_buf, 0, sizeof(recv_buf));
        recv_buf_count = 0;
        return;
    }

    recv_buf[recv_buf_count] = d;

    if (recv_buf[2]+2 > CMD_MAX_LEN) {
        rf_driver_debug("UART invalid command size: %d\n", recv_buf[2]);
        memset(recv_buf, 0, sizeof(recv_buf));
        recv_buf_count= 0;
        return;
    }

    recv_buf_count++;
    if (recv_buf_count > 2) {
        if (recv_buf_count == (recv_buf[2] + 2)) {
            // full packet received
            enqueue_command(&recv_buf[2]);
            memset(recv_buf, 0, sizeof(recv_buf));
            recv_buf_count = 0;
        }
    }
}

static void enqueue_command(uint8_t *cmd)
{
    uint8_t checksum = compute_checksum(cmd + 2, cmd[0] - 2);
    if (checksum != cmd[1]) {
        // invalid checksum
        rf_driver_debug("Checksum: LEN:%x, SRC:%x, CUR:%x\n", cmd[0], cmd[1], checksum);
        return;
    }
    if ((cmd[2] != CMD_SET_LEDS) && (cmd[0] != 4)){
        rf_driver_debug("Command: not a valid command:%d or length:%d\n", cmd[2], cmd[0]);
        return;
    }

    // set leds
    rf_cmd_set_leds(cmd[3]);
}

static void process_report(report_item_t *item)
{
    uint8_t checksum = item->type;
    checksum += compute_checksum(&item->data[0], item->size);
    send_buf[0] = SYNC_BYTE_1;
    send_buf[1] = SYNC_BYTE_2;
    send_buf[2] = item->size+3;
    send_buf[3] = checksum;
    send_buf[4] = item->type;
    for (uint32_t i = 0; i < item->size; i++) {
        send_buf[5+i] = item->data[i];
    }

    HAL_UART_Transmit(&huart_rf, send_buf, item->size+5, 10);

    rf_driver_debug("send report: type=%d, size=%d\n", item->type, item->size);
}

static uint8_t compute_checksum(uint8_t *data, uint32_t size)
{
    uint8_t checksum = 0;
    for (uint32_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum;
}

static void rf_cmd_set_leds(uint8_t led)
{
    rf_driver_debug("Led set state:%d\n", led);
}

void rf_driver_init(bool use_rf)
{
    // turn on rf power
    #ifdef RF_POWER_PIN
    gpio_set_output_pushpull(RF_POWER_PIN);
    gpio_write_pin(RF_POWER_PIN, 1);
    wait_ms(10);
    #endif

    #ifdef RF_RESET_PIN
    gpio_set_output_pushpull(RF_RESET_PIN);
    gpio_write_pin(RF_RESET_PIN, 0);
    wait_ms(1);
    gpio_write_pin(RF_RESET_PIN, 1);
    #endif


    // pin for checking rf state
    #ifdef RF_STATE_PIN
    gpio_set_input_pulldown(RF_STATE_PIN);
    #endif

    report_queue_init(&report_queue);

    memset(recv_buf, 0, sizeof(recv_buf));
    recv_buf_count = 0;
    rb_init(&ring_buffer, ring_buffer_data, 128);

    __HAL_UART_ENABLE_IT(&huart_rf, UART_IT_RXNE);

    if (use_rf && !(usb_setting&USB_OUTPUT_RF)) {
        rf_driver_toggle();
    }
}

void rf_driver_task(void)
{
    while (rb_used_count(&ring_buffer) > 0) {
        uint8_t c = 0;
        __disable_irq();
        c = rb_read_byte(&ring_buffer);
        __enable_irq();

        process_data(c);
    }

    //if (gpio_read_pin(RF_READY_PIN)) {
        report_item_t item;
        while (report_queue_get(&report_queue, &item)) {
            process_report(&item);
        }
    //}
}

void rf_driver_put_report(uint32_t type, void* data, uint32_t size)
{
    report_item_t item;
    memcpy(&item.data[0], data, size);
    item.size = size;
    item.type = type;

    report_queue_put(&report_queue, &item);
}

void rf_driver_toggle(void)
{
    if (usb_setting & USB_OUTPUT_RF) {
        usb_setting &= ~USB_OUTPUT_RF;
        rf_driver_debug("disable rf output\n");
    } else {
        usb_setting |= USB_OUTPUT_RF;
        rf_driver_debug("enable rf output\n");
    }
}

void rf_driver_erase_bond(void)
{
    uint8_t checksum = CMD_ERASE_BOND;
    send_buf[0] = SYNC_BYTE_1;
    send_buf[1] = SYNC_BYTE_2;
    send_buf[2] = 3;
    send_buf[3] = checksum;
    send_buf[4] = CMD_ERASE_BOND;

    HAL_UART_Transmit(&huart_rf, send_buf, 5, 10);

    rf_driver_debug("send erase bond\n");
}

void rf_driver_select_peer(int peer)
{
    uint8_t checksum = (uint8_t)(CMD_SELECT_PEER+peer);
    send_buf[0] = SYNC_BYTE_1;
    send_buf[1] = SYNC_BYTE_2;
    send_buf[2] = 3;
    send_buf[3] = checksum;
    send_buf[4] = CMD_SELECT_PEER;
    send_buf[5] = (uint8_t)peer;

    HAL_UART_Transmit(&huart_rf, send_buf, 6, 10);

    rf_driver_debug("send erase bond\n");
}

void uart_recv_char(uint8_t c)
{
    rb_write_byte(&ring_buffer, c);
}
