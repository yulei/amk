#include <stdbool.h>
#include "wait.h"
#include "matrix.h"
#include "bootloader.h"
#include "keymap.h"
#include "actionmap.h"
#include "host.h"
#include "action_layer.h"
#include "eeconfig.h"
#include "amk_bootmagic.h"
#include "amk_printf.h"

void bootmagic(void)
{
    /* check signature */
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    /* do scans in case of bounce */
    amk_printf("bootmagic scan: ... ");
    uint8_t scan = 100;
    while (scan--) { matrix_scan(); wait_ms(10); }
    amk_printf("done.\n");

    /* jump to bootloader */
    if (matrix_get_row(0) & 0x01) {
        // matrix(0,0)
        amk_printf("bootmagic: jump to bootloader \n");
        bootloader_jump();
    }

    /* bootmagic skip */
    if (bootmagic_scan_key(BM_KEY_SKIP)) {
        return;
    }

    /* eeconfig clear */
    if (bootmagic_scan_key(BM_KEY_EEPROM_CLEAR)) {
        eeconfig_init();
    }

    /* bootloader */
    if (bootmagic_scan_key(BM_KEY_BOOTLOADER)) {
        bootloader_jump();
    }
}

static bool scan_key(uint16_t code)
{
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        matrix_row_t matrix_row = matrix_get_row(r);
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            if (matrix_row & ((matrix_row_t)1<<c)) {
                // read key from Layer 0
                action_t action = action_for_key(0, (keypos_t){ .row = r, .col = c });
                if (action.code == code ||
                    ((action.kind.id == ACT_LMODS ||
                      action.kind.id == ACT_RMODS ||
                      action.kind.id == ACT_LMODS_TAP ||
                      action.kind.id == ACT_RMODS_TAP ||
                      action.kind.id == ACT_LAYER_TAP ||
                      action.kind.id == ACT_LAYER_TAP_EXT) &&
                     action.key.code == code)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool bootmagic_scan_key(uint16_t code)
{
    if (!scan_key(BM_KEY_SALT)) return false;

    return scan_key(code);
}
