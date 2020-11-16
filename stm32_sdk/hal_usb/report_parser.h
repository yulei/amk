/**
 * @file report_parser.h
 *
 *  simple hid report descriptor parser, the main goal was support tmk/qmk based keyboard
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

void init_report_parser(void);
bool parse_report_descriptor(uint32_t itf, const void* data, uint32_t size);
void report_decode(uint32_t itf, const void* data, uint32_t size);