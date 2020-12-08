/**
 * report_queue.c
 */

#include <string.h>
#include <stddef.h>
#include "report_queue.h"

uint32_t hid_report_queue_size(hid_report_queue_t* queue)
{
    if (queue->head <= queue->tail) {
        return queue->tail - queue->head;
    } else {
        return queue->tail + HID_REPORT_QUEUE_SIZE - queue->head;
    }
}

bool hid_report_queue_empty(hid_report_queue_t* queue)
{
    return queue->head == queue->tail;
}

bool hid_report_queue_full(hid_report_queue_t* queue)
{
    return ((queue->tail + 1) % HID_REPORT_QUEUE_SIZE) == queue->head;
}

void hid_report_queue_init(hid_report_queue_t* queue)
{
    memset(&queue->items[0], 0, sizeof(queue->items));
    queue->head = queue->tail = 0;
}

bool hid_report_queue_put(hid_report_queue_t* queue, hid_report_t* item)
{
    if (hid_report_queue_full(queue)) return false;

    queue->items[queue->tail] = *item;
    queue->tail = (queue->tail + 1) % HID_REPORT_QUEUE_SIZE;
    return true;
}

bool hid_report_queue_get(hid_report_queue_t* queue, hid_report_t* item)
{
    if (hid_report_queue_empty(queue)) return false;

    *item = queue->items[queue->head];
    queue->head = (queue->head + 1) % HID_REPORT_QUEUE_SIZE;
    return true;
}

hid_report_t *hid_report_queue_peek(hid_report_queue_t* queue)
{
    if (hid_report_queue_empty(queue)) return NULL;
    return &(queue->items[queue->head]);
}
