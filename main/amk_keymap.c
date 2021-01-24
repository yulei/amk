/**
 * @file amk_keymap.c
 */

#include <string.h>

#include "amk_keymap.h"
#include "eeprom_manager.h"

#include "action.h"
#include "keymap.h"
#include "amk_printf.h"

#ifdef WEBUSB_ENABLE

static uint16_t amk_keymaps[AMK_KEYMAP_MAX_LAYER][MATRIX_ROWS][MATRIX_COLS];

#ifdef ACTIONMAP_ENABLE
extern const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS];
#else
extern const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
extern const action_t fn_actions[];
#endif

#ifdef ACTIONMAP_ENABLE
action_t action_for_key(uint8_t layer, keypos_t key)
{
    return (action_t)amk_keymap_get(layer, key.row, key.col);
}

/* Macro */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    return MACRO_NONE;
}
#else
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return amk_keymap_get(layer, key.row, key.col);
}

action_t keymap_fn_to_action(uint8_t keycode)
{
    return fn_actions[FN_INDEX(keycode)];
}
#endif


void amk_keymap_init(void)
{
    memset(amk_keymaps, 0, sizeof(amk_keymaps));
    if (ee_keymap_is_valid()) {
        for(int layer = 0; layer < AMK_KEYMAP_MAX_LAYER; layer++) {
            for (int row = 0; row < MATRIX_ROWS; row++) {
                for (int col = 0; col < MATRIX_COLS; col++) {
                    amk_keymaps[layer][row][col] = ee_keymap_read_key(layer, row, col);
                }
            }
        }
    } else {
        for(int layer = 0; layer < AMK_KEYMAP_MAX_LAYER; layer++) {
            for (int row = 0; row < MATRIX_ROWS; row++) {
                for (int col = 0; col < MATRIX_COLS; col++) {
#ifdef ACTIONMAP_ENABLE
                    amk_keymaps[layer][row][col] = actionmaps[layer][row][col].code;
#else
                    amk_keymaps[layer][row][col] = keymaps[layer][row][col];
#endif
                    ee_keymap_write_key(layer, row, col, amk_keymaps[layer][row][col]);
                }
            }
            //ee_keymap_write(layer, &(amk_keymaps[layer][0][0]), MATRIX_ROWS*MATRIX_COLS*2);
        }
        ee_keymap_set_valid(true);
    }
}

void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    if (amk_keymaps[layer][row][col] == keycode) return;

    amk_printf("amk_keymap_set: layer=%d, row=%d, col=%d, key=0x%x\n", layer, row, col, keycode);
    amk_keymaps[layer][row][col] = keycode;
    ee_keymap_write_key(layer, row, col, amk_keymaps[layer][row][col]);
    //ee_keymap_write(layer, &(amk_keymaps[layer][0][0]), MATRIX_ROWS*MATRIX_COLS*2);
}

uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col)
{
    amk_printf("amk_keymap_get: layer=%d, row=%d, col=%d, key=0x%x\n", layer, row, col, amk_keymaps[layer][row][col]);
    return amk_keymaps[layer][row][col];
}

#else
void amk_keymap_init(void){}
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode) {}
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col) {return 0;}

#endif