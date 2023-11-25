/**
 * @file amk_keycode.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "quantum.h"

#define AMK_KEYCODE_START   QK_KB
enum amk_keycode {
    CUSTOM_START = AMK_KEYCODE_START-1,
#ifdef USE_HS_USB
    POLL_1K,                        // set polling rate to 1K hz
    POLL_2K,                        // set polling rate to 2K hz
    POLL_4K,                        // set polling rate to 4K hz
    POLL_8K,                        // set polling rate to 8K hz
#endif
#ifdef STATE_SCAN_ENABLE
    DOWN_0MS,                       // set press debounce to 0 ms
    DOWN_1MS,                       // set press debounce to 1 ms
    DOWN_2MS,                       // set press debounce to 2 ms
    DOWN_3MS,                       // set press debounce to 3 ms
    DOWN_4MS,                       // set press debounce to 4 ms
    DOWN_5MS,                       // set press debounce to 5 ms
    UP_0MS,                         // set release debounce to 0 ms
    UP_1MS,                         // set release debounce to 1 ms
    UP_2MS,                         // set release debounce to 2 ms
    UP_3MS,                         // set release debounce to 3 ms
    UP_4MS,                         // set release debounce to 4 ms
    UP_5MS,                         // set release debounce to 5 ms
#endif
};