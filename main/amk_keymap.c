/**
 * @file amk_keymap.c
 */

#include <string.h>

#include "amk_keymap.h"

#include "action.h"
#include "keymap.h"


static uint8_t amk_keymaps[AMK_KEYMAP_MAX_LAYER][MATRIX_ROWS][MATRIX_COLS];

extern const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
extern const uint32_t keymaps_size;
extern const action_t fn_actions[];

uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return amk_keymaps[(layer)][(key.row)][(key.col)];
}

action_t keymap_fn_to_action(uint8_t keycode)
{
    return fn_actions[FN_INDEX(keycode)];
}


void amk_keymap_init(void)
{
    memset(amk_keymaps, 0, sizeof(amk_keymaps));
    memcpy(amk_keymaps, keymaps, keymaps_size);
}

void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    amk_keymaps[layer][row][col] = (uint8_t)keycode;
}

uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col)
{
    return amk_keymaps[layer][row][col];
}