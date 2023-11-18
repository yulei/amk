/**
 * @file amk_keycode.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "quantum.h"

#define AMK_KEYCODE_START   QK_KB
enum amk_keycode {
    POLL_1K = AMK_KEYCODE_START,    // set polling rate to 1K hz
    POLL_2K,                        // set polling rate to 2K hz
    POLL_4K,                        // set polling rate to 4K hz
    POLL_8K,                        // set polling rate to 8K hz
};