/**
 * @file linear_buffer.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifndef LINEAR_BUFFER_MAX
#define LINEAR_BUFFER_MAX   128
#endif

typedef struct {
    uint8_t data[LINEAR_BUFFER_MAX];
    uint32_t size;
} linear_buf_t;

void linear_buf_init(linear_buf_t *cmd_buf);
bool linear_buf_full(linear_buf_t *cmd_buf);
bool linear_buf_put(linear_buf_t *cmd_buf, uint8_t data);
uint32_t linear_buf_size(linear_buf_t *cmd_buf);
void* linear_buf_data(linear_buf_t *cmd_buf);
bool linear_buf_get(linear_buf_t *buf, uint32_t offset, uint8_t *data);
