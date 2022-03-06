/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file report_queue.h
 * @brief queue for caching report
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

// maximum size of the queue item
#define HID_QUEUE_ITEM_SIZE         32

typedef struct {
    uint16_t    type;
    uint16_t    size;
    uint8_t     data[HID_QUEUE_ITEM_SIZE];
} hid_report_t;

// maximum size of the queue
#define HID_REPORT_QUEUE_SIZE      32

typedef struct {
    hid_report_t    items[HID_REPORT_QUEUE_SIZE];
    uint32_t        head;
    uint32_t        tail;
} hid_report_queue_t;

uint32_t hid_report_queue_size(hid_report_queue_t* queue);
bool hid_report_queue_empty(hid_report_queue_t* queue);
bool hid_report_queue_full(hid_report_queue_t* queue);
void hid_report_queue_init(hid_report_queue_t* queue);
bool hid_report_queue_put(hid_report_queue_t* queue, hid_report_t *item);
bool hid_report_queue_get(hid_report_queue_t* queue, hid_report_t *item);
hid_report_t *hid_report_queue_peek(hid_report_queue_t* queue);
void hid_report_queue_pop(hid_report_queue_t* queue);
