/**
 * @file report_parser.h
 *
 *  simple hid report descriptor parser, the main goal was support tmk/qmk based keyboard
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

bool parse_report_descriptor(uint32_t itf, const void* data, uint32_t size);
void repport_decode(uint32_t itf, const void* data, uint32_t size);