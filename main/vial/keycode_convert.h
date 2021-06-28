/**
 * @file keycode_convert.h
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

// qmk keycode definitions
// from quantum_keycodes.h
#define _______ KC_TRNS
#define XXXXXXX KC_NO

enum quantum_keycodes {
    // Ranges used in shortcuts - not to be used directly
    QK_BASIC                = 0x0000,
    QK_BASIC_MAX            = 0x00FF,
    QK_MODS                 = 0x0100,
    QK_LCTL                 = 0x0100,
    QK_LSFT                 = 0x0200,
    QK_LALT                 = 0x0400,
    QK_LGUI                 = 0x0800,
    QK_RMODS_MIN            = 0x1000,
    QK_RCTL                 = 0x1100,
    QK_RSFT                 = 0x1200,
    QK_RALT                 = 0x1400,
    QK_RGUI                 = 0x1800,
    QK_MODS_MAX             = 0x1FFF,
    QK_FUNCTION             = 0x2000,
    QK_FUNCTION_MAX         = 0x2FFF,
    QK_MACRO                = 0x3000,
    QK_MACRO_MAX            = 0x3FFF,
    QK_LAYER_TAP            = 0x4000,
    QK_LAYER_TAP_MAX        = 0x4FFF,
    QK_TO                   = 0x5000,
    QK_TO_MAX               = 0x50FF,
    QK_MOMENTARY            = 0x5100,
    QK_MOMENTARY_MAX        = 0x51FF,
    QK_DEF_LAYER            = 0x5200,
    QK_DEF_LAYER_MAX        = 0x52FF,
    QK_TOGGLE_LAYER         = 0x5300,
    QK_TOGGLE_LAYER_MAX     = 0x53FF,
    QK_ONE_SHOT_LAYER       = 0x5400,
    QK_ONE_SHOT_LAYER_MAX   = 0x54FF,
    QK_ONE_SHOT_MOD         = 0x5500,
    QK_ONE_SHOT_MOD_MAX     = 0x55FF,
    QK_TAP_DANCE            = 0x5700,
    QK_TAP_DANCE_MAX        = 0x57FF,
    QK_LAYER_TAP_TOGGLE     = 0x5800,
    QK_LAYER_TAP_TOGGLE_MAX = 0x58FF,
    QK_LAYER_MOD            = 0x5900,
    QK_LAYER_MOD_MAX        = 0x59FF,
#ifdef STENO_ENABLE
    QK_STENO        = 0x5A00,
    QK_STENO_BOLT   = 0x5A30,
    QK_STENO_GEMINI = 0x5A31,
    QK_STENO_MAX    = 0x5A3F,
#endif
#ifdef SWAP_HANDS_ENABLE
    QK_SWAP_HANDS     = 0x5B00,
    QK_SWAP_HANDS_MAX = 0x5BFF,
#endif
    QK_MOD_TAP     = 0x6000,
    QK_MOD_TAP_MAX = 0x7FFF,
#ifdef UNICODE_ENABLE
    QK_UNICODE     = 0x8000,
    QK_UNICODE_MAX = 0xFFFF,
#endif
#ifdef UNICODEMAP_ENABLE
    QK_UNICODEMAP          = 0x8000,
    QK_UNICODEMAP_MAX      = 0xBFFF,
    QK_UNICODEMAP_PAIR     = 0xC000,
    QK_UNICODEMAP_PAIR_MAX = 0xFFFF,
#endif

    // Loose keycodes - to be used directly

    QK_RESET = 0x5C00,
    QK_DEBUG,
    MAGIC_SWAP_CONTROL_CAPSLOCK,
    MAGIC_CAPSLOCK_TO_CONTROL,
    MAGIC_SWAP_LALT_LGUI,
    MAGIC_SWAP_RALT_RGUI,
    MAGIC_NO_GUI,
    MAGIC_SWAP_GRAVE_ESC,
    MAGIC_SWAP_BACKSLASH_BACKSPACE,
    MAGIC_HOST_NKRO,
    MAGIC_SWAP_ALT_GUI,
    MAGIC_UNSWAP_CONTROL_CAPSLOCK,
    MAGIC_UNCAPSLOCK_TO_CONTROL,
    MAGIC_UNSWAP_LALT_LGUI,
    MAGIC_UNSWAP_RALT_RGUI,
    MAGIC_UNNO_GUI,
    MAGIC_UNSWAP_GRAVE_ESC,
    MAGIC_UNSWAP_BACKSLASH_BACKSPACE,
    MAGIC_UNHOST_NKRO,
    MAGIC_UNSWAP_ALT_GUI,
    MAGIC_TOGGLE_NKRO,
    MAGIC_TOGGLE_ALT_GUI,
    GRAVE_ESC,

// Leader key
#ifdef LEADER_ENABLE
    KC_LEAD,
#endif

// Auto Shift setup
#ifndef AUTO_SHIFT_NO_SETUP
    KC_ASUP,
    KC_ASDN,
    KC_ASRP,
#endif
    KC_ASTG,
    KC_ASON,
    KC_ASOFF,

    // Audio on/off/toggle
    AU_ON,
    AU_OFF,
    AU_TOG,

    // Faux clicky as part of main audio feature
    CLICKY_TOGGLE,
    CLICKY_ENABLE,
    CLICKY_DISABLE,
    CLICKY_UP,
    CLICKY_DOWN,
    CLICKY_RESET,

    // Music mode on/off/toggle
    MU_ON,
    MU_OFF,
    MU_TOG,

    // Music mode cycle
    MU_MOD,

    // Music voice iterate
    MUV_IN,
    MUV_DE,

// Midi
#if !MIDI_ENABLE_STRICT || (defined(MIDI_ENABLE) && defined(MIDI_BASIC))
    MI_ON,
    MI_OFF,
    MI_TOG,
#endif

#if !MIDI_ENABLE_STRICT || (defined(MIDI_ENABLE) && defined(MIDI_ADVANCED))
    MIDI_TONE_MIN,

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 0
    MI_C = MIDI_TONE_MIN,
    MI_Cs,
    MI_Db = MI_Cs,
    MI_D,
    MI_Ds,
    MI_Eb = MI_Ds,
    MI_E,
    MI_F,
    MI_Fs,
    MI_Gb = MI_Fs,
    MI_G,
    MI_Gs,
    MI_Ab = MI_Gs,
    MI_A,
    MI_As,
    MI_Bb = MI_As,
    MI_B,
#    endif

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 1
    MI_C_1,
    MI_Cs_1,
    MI_Db_1 = MI_Cs_1,
    MI_D_1,
    MI_Ds_1,
    MI_Eb_1 = MI_Ds_1,
    MI_E_1,
    MI_F_1,
    MI_Fs_1,
    MI_Gb_1 = MI_Fs_1,
    MI_G_1,
    MI_Gs_1,
    MI_Ab_1 = MI_Gs_1,
    MI_A_1,
    MI_As_1,
    MI_Bb_1 = MI_As_1,
    MI_B_1,
#    endif

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 2
    MI_C_2,
    MI_Cs_2,
    MI_Db_2 = MI_Cs_2,
    MI_D_2,
    MI_Ds_2,
    MI_Eb_2 = MI_Ds_2,
    MI_E_2,
    MI_F_2,
    MI_Fs_2,
    MI_Gb_2 = MI_Fs_2,
    MI_G_2,
    MI_Gs_2,
    MI_Ab_2 = MI_Gs_2,
    MI_A_2,
    MI_As_2,
    MI_Bb_2 = MI_As_2,
    MI_B_2,
#    endif

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 3
    MI_C_3,
    MI_Cs_3,
    MI_Db_3 = MI_Cs_3,
    MI_D_3,
    MI_Ds_3,
    MI_Eb_3 = MI_Ds_3,
    MI_E_3,
    MI_F_3,
    MI_Fs_3,
    MI_Gb_3 = MI_Fs_3,
    MI_G_3,
    MI_Gs_3,
    MI_Ab_3 = MI_Gs_3,
    MI_A_3,
    MI_As_3,
    MI_Bb_3 = MI_As_3,
    MI_B_3,
#    endif

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 4
    MI_C_4,
    MI_Cs_4,
    MI_Db_4 = MI_Cs_4,
    MI_D_4,
    MI_Ds_4,
    MI_Eb_4 = MI_Ds_4,
    MI_E_4,
    MI_F_4,
    MI_Fs_4,
    MI_Gb_4 = MI_Fs_4,
    MI_G_4,
    MI_Gs_4,
    MI_Ab_4 = MI_Gs_4,
    MI_A_4,
    MI_As_4,
    MI_Bb_4 = MI_As_4,
    MI_B_4,
#    endif

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 5
    MI_C_5,
    MI_Cs_5,
    MI_Db_5 = MI_Cs_5,
    MI_D_5,
    MI_Ds_5,
    MI_Eb_5 = MI_Ds_5,
    MI_E_5,
    MI_F_5,
    MI_Fs_5,
    MI_Gb_5 = MI_Fs_5,
    MI_G_5,
    MI_Gs_5,
    MI_Ab_5 = MI_Gs_5,
    MI_A_5,
    MI_As_5,
    MI_Bb_5 = MI_As_5,
    MI_B_5,
#    endif

#    if !MIDI_ENABLE_STRICT || MIDI_TONE_KEYCODE_OCTAVES > 5
    MIDI_TONE_MAX = MI_B_5,
#    elif MIDI_TONE_KEYCODE_OCTAVES > 4
    MIDI_TONE_MAX = MI_B_4,
#    elif MIDI_TONE_KEYCODE_OCTAVES > 3
    MIDI_TONE_MAX = MI_B_3,
#    elif MIDI_TONE_KEYCODE_OCTAVES > 2
    MIDI_TONE_MAX = MI_B_2,
#    elif MIDI_TONE_KEYCODE_OCTAVES > 1
    MIDI_TONE_MAX = MI_B_1,
#    elif MIDI_TONE_KEYCODE_OCTAVES > 0
    MIDI_TONE_MAX = MI_B,
#    endif

    MIDI_OCTAVE_MIN,
    MI_OCT_N2 = MIDI_OCTAVE_MIN,
    MI_OCT_N1,
    MI_OCT_0,
    MI_OCT_1,
    MI_OCT_2,
    MI_OCT_3,
    MI_OCT_4,
    MI_OCT_5,
    MI_OCT_6,
    MI_OCT_7,
    MIDI_OCTAVE_MAX = MI_OCT_7,
    MI_OCTD,  // octave down
    MI_OCTU,  // octave up

    MIDI_TRANSPOSE_MIN,
    MI_TRNS_N6 = MIDI_TRANSPOSE_MIN,
    MI_TRNS_N5,
    MI_TRNS_N4,
    MI_TRNS_N3,
    MI_TRNS_N2,
    MI_TRNS_N1,
    MI_TRNS_0,
    MI_TRNS_1,
    MI_TRNS_2,
    MI_TRNS_3,
    MI_TRNS_4,
    MI_TRNS_5,
    MI_TRNS_6,
    MIDI_TRANSPOSE_MAX = MI_TRNS_6,
    MI_TRNSD,  // transpose down
    MI_TRNSU,  // transpose up

    MIDI_VELOCITY_MIN,
    MI_VEL_0 = MIDI_VELOCITY_MIN,
    MI_VEL_1 = MIDI_VELOCITY_MIN,
    MI_VEL_2,
    MI_VEL_3,
    MI_VEL_4,
    MI_VEL_5,
    MI_VEL_6,
    MI_VEL_7,
    MI_VEL_8,
    MI_VEL_9,
    MI_VEL_10,
    MIDI_VELOCITY_MAX = MI_VEL_10,
    MI_VELD,  // velocity down
    MI_VELU,  // velocity up

    MIDI_CHANNEL_MIN,
    MI_CH1 = MIDI_CHANNEL_MIN,
    MI_CH2,
    MI_CH3,
    MI_CH4,
    MI_CH5,
    MI_CH6,
    MI_CH7,
    MI_CH8,
    MI_CH9,
    MI_CH10,
    MI_CH11,
    MI_CH12,
    MI_CH13,
    MI_CH14,
    MI_CH15,
    MI_CH16,
    MIDI_CHANNEL_MAX = MI_CH16,
    MI_CHD,  // previous channel
    MI_CHU,  // next channel

    MI_ALLOFF,  // all notes off

    MI_SUS,   // sustain
    MI_PORT,  // portamento
    MI_SOST,  // sostenuto
    MI_SOFT,  // soft pedal
    MI_LEG,   // legato

    MI_MOD,    // modulation
    MI_MODSD,  // decrease modulation speed
    MI_MODSU,  // increase modulation speed

    MI_BENDD,  // Bend down
    MI_BENDU,  // Bend up
#endif         // MIDI_ADVANCED

    // Backlight functionality
    BL_ON,
    BL_OFF,
    BL_DEC,
    BL_INC,
    BL_TOGG,
    BL_STEP,
    BL_BRTG,

    // RGB functionality
    RGB_TOG,
    RGB_MODE_FORWARD,
    RGB_MODE_REVERSE,
    RGB_HUI,
    RGB_HUD,
    RGB_SAI,
    RGB_SAD,
    RGB_VAI,
    RGB_VAD,
    RGB_SPI,
    RGB_SPD,
    RGB_MODE_PLAIN,
    RGB_MODE_BREATHE,
    RGB_MODE_RAINBOW,
    RGB_MODE_SWIRL,
    RGB_MODE_SNAKE,
    RGB_MODE_KNIGHT,
    RGB_MODE_XMAS,
    RGB_MODE_GRADIENT,
    RGB_MODE_RGBTEST,

    // Momentum matching toggle
    VLK_TOG,

    // Left shift, open paren
    KC_LSPO,

    // Right shift, close paren
    KC_RSPC,

    // Shift, Enter
    KC_SFTENT,

    // Printing
    PRINT_ON,
    PRINT_OFF,

    // output selection
    OUT_AUTO,
    OUT_USB,
#ifdef BLUETOOTH_ENABLE
    OUT_BT,
#endif

#ifdef KEY_LOCK_ENABLE
    KC_LOCK,
#endif

#ifdef TERMINAL_ENABLE
    TERM_ON,
    TERM_OFF,
#endif

    EEPROM_RESET,

    UNICODE_MODE_FORWARD,
    UNICODE_MODE_REVERSE,
    UNICODE_MODE_MAC,
    UNICODE_MODE_LNX,
    UNICODE_MODE_WIN,
    UNICODE_MODE_BSD,
    UNICODE_MODE_WINC,

    HPT_ON,
    HPT_OFF,
    HPT_TOG,
    HPT_RST,
    HPT_FBK,
    HPT_BUZ,
    HPT_MODI,
    HPT_MODD,
    HPT_CONT,
    HPT_CONI,
    HPT_COND,
    HPT_DWLI,
    HPT_DWLD,

    // Left control, open paren
    KC_LCPO,

    // Right control, close paren
    KC_RCPC,

    // Left control, open paren
    KC_LAPO,

    // Right control, close paren
    KC_RAPC,

    CMB_ON,
    CMB_OFF,
    CMB_TOG,

    MAGIC_SWAP_LCTL_LGUI,
    MAGIC_SWAP_RCTL_RGUI,
    MAGIC_UNSWAP_LCTL_LGUI,
    MAGIC_UNSWAP_RCTL_RGUI,
    MAGIC_SWAP_CTL_GUI,
    MAGIC_UNSWAP_CTL_GUI,
    MAGIC_TOGGLE_CTL_GUI,
    MAGIC_EE_HANDS_LEFT,
    MAGIC_EE_HANDS_RIGHT,

    // Dynamic Macros
    DYN_REC_START1,
    DYN_REC_START2,
    DYN_REC_STOP,
    DYN_MACRO_PLAY1,
    DYN_MACRO_PLAY2,

    JS_BUTTON0,
    JS_BUTTON_MIN = JS_BUTTON0,
    JS_BUTTON1,
    JS_BUTTON2,
    JS_BUTTON3,
    JS_BUTTON4,
    JS_BUTTON5,
    JS_BUTTON6,
    JS_BUTTON7,
    JS_BUTTON8,
    JS_BUTTON9,
    JS_BUTTON10,
    JS_BUTTON11,
    JS_BUTTON12,
    JS_BUTTON13,
    JS_BUTTON14,
    JS_BUTTON15,
    JS_BUTTON16,
    JS_BUTTON17,
    JS_BUTTON18,
    JS_BUTTON19,
    JS_BUTTON20,
    JS_BUTTON21,
    JS_BUTTON22,
    JS_BUTTON23,
    JS_BUTTON24,
    JS_BUTTON25,
    JS_BUTTON26,
    JS_BUTTON27,
    JS_BUTTON28,
    JS_BUTTON29,
    JS_BUTTON30,
    JS_BUTTON31,
    JS_BUTTON_MAX = JS_BUTTON31,

#if defined(SEQUENCER_ENABLE)
    SQ_ON,
    SQ_OFF,
    SQ_TOG,

    SQ_TMPD,  // Decrease tempo
    SQ_TMPU,  // Increase tempo

    SEQUENCER_RESOLUTION_MIN,
    SEQUENCER_RESOLUTION_MAX = SEQUENCER_RESOLUTION_MIN + SEQUENCER_RESOLUTIONS,
    SQ_RESD,  // Decrease resolution
    SQ_RESU,  // Increase resolution

    SQ_SALL,  // All steps on
    SQ_SCLR,  // All steps off
    SEQUENCER_STEP_MIN,
    SEQUENCER_STEP_MAX = SEQUENCER_STEP_MIN + SEQUENCER_STEPS,

    SEQUENCER_TRACK_MIN,
    SEQUENCER_TRACK_MAX = SEQUENCER_TRACK_MIN + SEQUENCER_TRACKS,

/**
 * Helpers to assign a keycode to a step, a resolution, or a track.
 * Falls back to NOOP if n is out of range.
 */
#    define SQ_S(n) (n < SEQUENCER_STEPS ? SEQUENCER_STEP_MIN + n : XXXXXXX)
#    define SQ_R(n) (n < SEQUENCER_RESOLUTIONS ? SEQUENCER_RESOLUTION_MIN + n : XXXXXXX)
#    define SQ_T(n) (n < SEQUENCER_TRACKS ? SEQUENCER_TRACK_MIN + n : XXXXXXX)

#endif

    // always leave at the end
    SAFE_RANGE
};


// Can't use SAFE_RANGE here, it might change if someone adds
// new values to enum quantum_keycodes.
// Need to keep checking 0x5F10 is still in the safe range.
// TODO: merge this into quantum_keycodes
// Backlight keycodes are in range 0x5F00-0x5F0F
enum via_keycodes {
    FN_MO13 = 0x5F10,
    FN_MO23,
    MACRO00,
    MACRO01,
    MACRO02,
    MACRO03,
    MACRO04,
    MACRO05,
    MACRO06,
    MACRO07,
    MACRO08,
    MACRO09,
    MACRO10,
    MACRO11,
    MACRO12,
    MACRO13,
    MACRO14,
    MACRO15,
};

enum user_keycodes {
    USER00 = 0x5F80,
    USER01,
    USER02,
    USER03,
    USER04,
    USER05,
    USER06,
    USER07,
    USER08,
    USER09,
    USER10,
    USER11,
    USER12,
    USER13,
    USER14,
    USER15,
};

uint16_t amk_to_vial(uint16_t keycode);
uint16_t vial_to_amk(uint16_t keycode);
// convert keycode in place
void convert_keycode_buffer(uint8_t *keycodes, uint8_t size, bool amk);