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
#ifdef AMK_APC_ENABLE
    PROFILE_0,                      // set apcrt profile 0
    PROFILE_1,                      // set apcrt profile 1
    PROFILE_2,                      // set apcrt profile 2
    PROFILE_3,                      // set apcrt profile 3
#endif
#ifdef AMK_DKS_ENABLE
    DKS_TOGGLE,                     //  toggle dks on/off
#endif
};