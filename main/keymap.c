#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"

static const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    {{KC_CAPS}}, // test with KC_CAPS, KC_A, KC_BTLD
};

static const action_t fn_actions[] = {
};

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return keymaps[(layer)][(key.row)][(key.col)];
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    return (action_t)fn_actions[FN_INDEX(keycode)];
}
