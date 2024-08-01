/**
 * @file amk_snaptap.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "amk_snaptap.h"
#include "amk_apc.h"
#include "amk_store.h"
#include "amk_printf.h"

#ifndef SNAPTAP_DEBUG
#define SNAPTAP_DEBUG 1
#endif

#if SNAPTAP_DEBUG
#define snaptap_debug  amk_printf
#else
#define snaptap_debug(...)
#endif


static bool snaptap_on = true;
static snaptap_pair_t snaptaps[AMK_SNAPTAP_COUNT] = {0};

static void process_snaptap(matrix_row_t *raw, snaptap_pair_t* pair);

void snaptap_init(void)
{
    struct amk_snaptap key;
    for (int i = 0; i < AMK_SNAPTAP_COUNT; i++) {
        amk_store_get_snaptap(i, &key);
        snaptaps[i].first.row = key.first_row;
        snaptaps[i].first.col = key.first_col;
        snaptaps[i].second.row = key.second_row;
        snaptaps[i].second.col = key.second_col;
        snaptaps[i].mode = key.mode;
    }

    snaptap_on = (amk_store_get_snaptap_config() == 0) ? true : false;

    #if 0
    // load snaptap from eeprom
    snaptaps[0].first.row = 3;
    snaptaps[0].first.col = 13;
    snaptaps[0].second.row = 3;
    snaptaps[0].second.col = 15;
    snaptaps[0].mode = SNAPTAP_TRIGGER;
    #endif
}

bool snaptap_process(matrix_row_t *raw)
{
    if (!snaptap_on) return true;

    for (int i = 0; i < AMK_SNAPTAP_COUNT; i++) {
        snaptap_pair_t *pair = &snaptaps[i];
        if (pair->mode != SNAPTAP_NONE) {
            process_snaptap(raw, pair);
        }
   }

   return true;
}

void snaptap_update(uint8_t index, const snaptap_pair_t* pair)
{
    if (index < AMK_SNAPTAP_COUNT) {
        snaptaps[index] = *pair;
        snaptap_debug("SNAPTAP update at index(%d), first(%d,%d), second(%d,%d), mode(%d)\n", index, 
                        pair->first.row, pair->first.col, pair->second.row, pair->second.col);
    } else {
        snaptap_debug("SNATAP update out of bound (%d), max(%d)\n", index, AMK_SNAPTAP_COUNT);
    }
}

void process_snaptap(matrix_row_t *raw, snaptap_pair_t* pair)
{
    bool first_on = false;
    uint32_t first_value = 0;
    uint32_t first_ticks = 0;
    apc_matrix_get_key(pair->first.row, pair->first.col, &first_on, &first_value, &first_ticks);
    bool second_on = false;
    uint32_t second_value = 0;
    uint32_t second_ticks = 0;
    apc_matrix_get_key(pair->second.row, pair->second.col, &second_on, &second_value, &second_ticks);
    if (!first_on || !second_on) return;

    switch(pair->mode) {
        case SNAPTAP_DEPTH:
            {
                uint8_t col = 0;
                uint8_t row = 0;
                if (first_value > second_value) {
                    col = pair->second.col;
                    row = pair->second.row;
                } else {
                    col = pair->first.col;
                    row = pair->first.row;
                }
                // clear the key in the matrix 
                raw[row] &= ~(1<<col);
                snaptap_debug("SNATAP Depth: key(%d,%d) cleared\n", row, col); 
            }
            break;
        case SNAPTAP_TRIGGER:
            {
                uint8_t col = 0;
                uint8_t row = 0;
                if (first_ticks > second_ticks) {
                    col = pair->second.col;
                    row = pair->second.row;
                } else {
                    col = pair->first.col;
                    row = pair->first.row;
                }
                // clear the key in the matrix 
                raw[row] &= ~(1<<col);
                snaptap_debug("SNATAP Trigger: key(%d,%d) cleared\n", row, col); 
            }
            break;
        case SNAPTAP_FIRST:
            {
                uint8_t col = pair->second.col;
                uint8_t row = pair->second.row;

                // clear the key in the matrix 
                raw[row] &= ~(1<<col);
                snaptap_debug("SNATAP First: key(%d,%d) cleared\n", row, col); 
            }
            break;
        case SNAPTAP_SECOND:
            {
                uint8_t col = pair->first.col;
                uint8_t row = pair->first.row;

                // clear the key in the matrix 
                raw[row] &= ~(1<<col);
                snaptap_debug("SNATAP Second: key(%d,%d) cleared\n", row, col); 
            }
            break;
        case SNAPTAP_BOTH:
            {
                // clear both key in the matrix 
                raw[pair->first.row] &= ~(1<<pair->first.col);
                raw[pair->second.row] &= ~(1<<pair->second.col);
                snaptap_debug("SNATAP Both: first(%d,%d) and second(%d,%d), cleared\n", 
                                pair->first.row, pair->first.col, pair->second.row, pair->second.col); 
            }
            break;
        default:
            break;
    }
}

void snaptap_update_config(uint8_t config)
{
    snaptap_on = (config == 0) ? true : false;
}

void snaptap_toggle(void)
{
    snaptap_on = !snaptap_on;
    amk_store_set_snaptap_config(snaptap_on ? 0 : 1);
    snaptap_debug("SNATAP set enabled(%d)\n", snaptap_on); 
}