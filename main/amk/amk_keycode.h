/**
 * @file amk_keycode.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "amk_config.h"

#ifdef ACTIONMAP_ENABLE

/* Normal key */
#define GC_NO                   AC_NO
#define GC_TRANSPARENT          AC_TRANSPARENT
#define GC_ROLL_OVER            AC_ROLL_OVER
#define GC_POST_FAIL            AC_POST_FAIL
#define GC_UNDEFINED            AC_UNDEFINED
#define GC_A                    AC_A
#define GC_B                    AC_B
#define GC_C                    AC_C
#define GC_D                    AC_D
#define GC_E                    AC_E
#define GC_F                    AC_F
#define GC_G                    AC_G
#define GC_H                    AC_H
#define GC_I                    AC_I
#define GC_J                    AC_J
#define GC_K                    AC_K
#define GC_L                    AC_L
#define GC_M                    AC_M
#define GC_N                    AC_N
#define GC_O                    AC_O
#define GC_P                    AC_P
#define GC_Q                    AC_Q
#define GC_R                    AC_R
#define GC_S                    AC_S
#define GC_T                    AC_T
#define GC_U                    AC_U
#define GC_V                    AC_V
#define GC_W                    AC_W
#define GC_X                    AC_X
#define GC_Y                    AC_Y
#define GC_Z                    AC_Z
#define GC_1                    AC_1
#define GC_2                    AC_2
#define GC_3                    AC_3
#define GC_4                    AC_4
#define GC_5                    AC_5
#define GC_6                    AC_6
#define GC_7                    AC_7
#define GC_8                    AC_8
#define GC_9                    AC_9
#define GC_0                    AC_0
#define GC_ENTER                AC_ENTER
#define GC_ESCAPE               AC_ESCAPE
#define GC_BSPACE               AC_BSPACE
#define GC_TAB                  AC_TAB
#define GC_SPACE                AC_SPACE
#define GC_MINUS                AC_MINUS
#define GC_EQUAL                AC_EQUAL
#define GC_LBRACKET             AC_LBRACKET
#define GC_RBRACKET             AC_RBRACKET
#define GC_BSLASH               AC_BSLASH
#define GC_NONUS_HASH           AC_NONUS_HASH
#define GC_SCOLON               AC_SCOLON
#define GC_QUOTE                AC_QUOTE
#define GC_GRAVE                AC_GRAVE
#define GC_COMMA                AC_COMMA
#define GC_DOT                  AC_DOT
#define GC_SLASH                AC_SLASH
#define GC_CAPSLOCK             AC_CAPSLOCK
#define GC_F1                   AC_F1
#define GC_F2                   AC_F2
#define GC_F3                   AC_F3
#define GC_F4                   AC_F4
#define GC_F5                   AC_F5
#define GC_F6                   AC_F6
#define GC_F7                   AC_F7
#define GC_F8                   AC_F8
#define GC_F9                   AC_F9
#define GC_F10                  AC_F10
#define GC_F11                  AC_F11
#define GC_F12                  AC_F12
#define GC_PSCREEN              AC_PSCREEN
#define GC_SCROLLLOCK           AC_SCROLLLOCK
#define GC_PAUSE                AC_PAUSE
#define GC_INSERT               AC_INSERT
#define GC_HOME                 AC_HOME
#define GC_PGUP                 AC_PGUP
#define GC_DELETE               AC_DELETE
#define GC_END                  AC_END
#define GC_PGDOWN               AC_PGDOWN
#define GC_RIGHT                AC_RIGHT
#define GC_LEFT                 AC_LEFT
#define GC_DOWN                 AC_DOWN
#define GC_UP                   AC_UP
#define GC_NUMLOCK              AC_NUMLOCK
#define GC_KP_SLASH             AC_KP_SLASH
#define GC_KP_ASTERISK          AC_KP_ASTERISK
#define GC_KP_MINUS             AC_KP_MINUS
#define GC_KP_PLUS              AC_KP_PLUS
#define GC_KP_ENTER             AC_KP_ENTER
#define GC_KP_1                 AC_KP_1
#define GC_KP_2                 AC_KP_2
#define GC_KP_3                 AC_KP_3
#define GC_KP_4                 AC_KP_4
#define GC_KP_5                 AC_KP_5
#define GC_KP_6                 AC_KP_6
#define GC_KP_7                 AC_KP_7
#define GC_KP_8                 AC_KP_8
#define GC_KP_9                 AC_KP_9
#define GC_KP_0                 AC_KP_0
#define GC_KP_DOT               AC_KP_DOT
#define GC_NONUS_BSLASH         AC_NONUS_BSLASH
#define GC_APPLICATION          AC_APPLICATION
#define GC_POWER                AC_POWER
#define GC_KP_EQUAL             AC_KP_EQUAL
#define GC_F13                  AC_F13
#define GC_F14                  AC_F14
#define GC_F15                  AC_F15
#define GC_F16                  AC_F16
#define GC_F17                  AC_F17
#define GC_F18                  AC_F18
#define GC_F19                  AC_F19
#define GC_F20                  AC_F20
#define GC_F21                  AC_F21
#define GC_F22                  AC_F22
#define GC_F23                  AC_F23
#define GC_F24                  AC_F24
#define GC_EXECUTE              AC_EXECUTE
#define GC_HELP                 AC_HELP
#define GC_MENU                 AC_MENU
#define GC_SELECT               AC_SELECT
#define GC_STOP                 AC_STOP
#define GC_AGAIN                AC_AGAIN
#define GC_UNDO                 AC_UNDO
#define GC_CUT                  AC_CUT
#define GC_COPY                 AC_COPY
#define GC_PASTE                AC_PASTE
#define GC_FIND                 AC_FIND
#define GC__MUTE                AC__MUTE
#define GC__VOLUP               AC__VOLUP
#define GC__VOLDOWN             AC__VOLDOWN
#define GC_LOCKING_CAPS         AC_LOCKING_CAPS
#define GC_LOCKING_NUM          AC_LOCKING_NUM
#define GC_LOCKING_SCROLL       AC_LOCKING_SCROLL
#define GC_KP_COMMA             AC_KP_COMMA
#define GC_KP_EQUAL_AS400       AC_KP_EQUAL_AS400
#define GC_INT1                 AC_INT1
#define GC_INT2                 AC_INT2
#define GC_INT3                 AC_INT3
#define GC_INT4                 AC_INT4
#define GC_INT5                 AC_INT5
#define GC_INT6                 AC_INT6
#define GC_INT7                 AC_INT7
#define GC_INT8                 AC_INT8
#define GC_INT9                 AC_INT9
#define GC_LANG1                AC_LANG1
#define GC_LANG2                AC_LANG2
#define GC_LANG3                AC_LANG3
#define GC_LANG4                AC_LANG4
#define GC_LANG5                AC_LANG5
#define GC_LANG6                AC_LANG6
#define GC_LANG7                AC_LANG7
#define GC_LANG8                AC_LANG8
#define GC_LANG9                AC_LANG9
#define GC_ALT_ERASE            AC_ALT_ERASE
#define GC_SYSREQ               AC_SYSREQ
#define GC_CANCEL               AC_CANCEL
#define GC_CLEAR                AC_CLEAR
#define GC_PRIOR                AC_PRIOR
#define GC_RETURN               AC_RETURN
#define GC_SEPARATOR            AC_SEPARATOR
#define GC_OUT                  AC_OUT
#define GC_OPER                 AC_OPER
#define GC_CLEAR_AGAIN          AC_CLEAR_AGAIN
#define GC_CRSEL                AC_CRSEL
#define GC_EXSEL                AC_EXSEL
#define GC_KP_00                AC_KP_00
#define GC_KP_000               AC_KP_000
#define GC_THOUSANDS_SEPARATOR  AC_THOUSANDS_SEPARATOR
#define GC_DECIMAL_SEPARATOR    AC_DECIMAL_SEPARATOR
#define GC_CURRENCY_UNIT        AC_CURRENCY_UNIT
#define GC_CURRENCY_SUB_UNIT    AC_CURRENCY_SUB_UNIT
#define GC_KP_LPAREN            AC_KP_LPAREN
#define GC_KP_RPAREN            AC_KP_RPAREN
#define GC_KP_LCBRACKET         AC_KP_LCBRACKET
#define GC_KP_RCBRACKET         AC_KP_RCBRACKET
#define GC_KP_TAB               AC_KP_TAB
#define GC_KP_BSPACE            AC_KP_BSPACE
#define GC_KP_A                 AC_KP_A
#define GC_KP_B                 AC_KP_B
#define GC_KP_C                 AC_KP_C
#define GC_KP_D                 AC_KP_D
#define GC_KP_E                 AC_KP_E
#define GC_KP_F                 AC_KP_F
#define GC_KP_XOR               AC_KP_XOR
#define GC_KP_HAT               AC_KP_HAT
#define GC_KP_PERC              AC_KP_PERC
#define GC_KP_LT                AC_KP_LT
#define GC_KP_GT                AC_KP_GT
#define GC_KP_AND               AC_KP_AND
#define GC_KP_LAZYAND           AC_KP_LAZYAND
#define GC_KP_OR                AC_KP_OR
#define GC_KP_LAZYOR            AC_KP_LAZYOR
#define GC_KP_COLON             AC_KP_COLON
#define GC_KP_HASH              AC_KP_HASH
#define GC_KP_SPACE             AC_KP_SPACE
#define GC_KP_ATMARK            AC_KP_ATMARK
#define GC_KP_EXCLAMATION       AC_KP_EXCLAMATION
#define GC_KP_MEM_STORE         AC_KP_MEM_STORE
#define GC_KP_MEM_RECALL        AC_KP_MEM_RECALL
#define GC_KP_MEM_CLEAR         AC_KP_MEM_CLEAR
#define GC_KP_MEM_ADD           AC_KP_MEM_ADD
#define GC_KP_MEM_SUB           AC_KP_MEM_SUB
#define GC_KP_MEM_MUL           AC_KP_MEM_MUL
#define GC_KP_MEM_DIV           AC_KP_MEM_DIV
#define GC_KP_PLUS_MINUS        AC_KP_PLUS_MINUS
#define GC_KP_CLEAR             AC_KP_CLEAR
#define GC_KP_CLEAR_ENTRY       AC_KP_CLEAR_ENTRY
#define GC_KP_BINARY            AC_KP_BINARY
#define GC_KP_OCTAL             AC_KP_OCTAL
#define GC_KP_DECIMAL           AC_KP_DECIMAL
#define GC_KP_HEXADECIMAL       AC_KP_HEXADECIMAL

/* Modifiers */
#define GC_LCTRL                AC_LCTRL
#define GC_LSHIFT               AC_LSHIFT
#define GC_LALT                 AC_LALT
#define GC_LGUI                 AC_LGUI
#define GC_RCTRL                AC_RCTRL
#define GC_RSHIFT               AC_RSHIFT
#define GC_RALT                 AC_RALT
#define GC_RGUI                 AC_RGUI

/* Sytem Control */
#define GC_SYSTEM_POWER         AC_SYSTEM_POWER
#define GC_SYSTEM_SLEEP         AC_SYSTEM_SLEEP
#define GC_SYSTEM_WAKE          AC_SYSTEM_WAKE

/* Consumer Page */
#define GC_AUDIO_MUTE           AC_AUDIO_MUTE
#define GC_AUDIO_VOL_UP         AC_AUDIO_VOL_UP
#define GC_AUDIO_VOL_DOWN       AC_AUDIO_VOL_DOWN
#define GC_MEDIA_NEXT_TRACK     AC_MEDIA_NEXT_TRACK
#define GC_MEDIA_PREV_TRACK     AC_MEDIA_PREV_TRACK
#define GC_MEDIA_FAST_FORWARD   AC_MEDIA_FAST_FORWARD
#define GC_MEDIA_REWIND         AC_MEDIA_REWIND
#define GC_MEDIA_STOP           AC_MEDIA_STOP
#define GC_PLAY_PAUSE           AC_MEDIA_PLAY_PAUSE
#define GC_MEDIA_EJECT          AC_MEDIA_EJECT
#define GC_MEDIA_SELECT         AC_MEDIA_SELECT
#define GC_MAIL                 AC_MAIL
#define GC_CALCULATOR           AC_CALCULATOR
#define GC_MY_COMPUTER          AC_MY_COMPUTER
#define GC_WWW_SEARCH           AC_WWW_SEARCH
#define GC_WWW_HOME             AC_WWW_HOME
#define GC_WWW_BACK             AC_WWW_BACK
#define GC_WWW_FORWARD          AC_WWW_FORWARD
#define GC_WWW_STOP             AC_WWW_STOP
#define GC_WWW_REFRESH          AC_WWW_REFRESH
#define GC_WWW_FAVORITES        AC_WWW_FAVORITES
#define GC_BRIGHTNESS_INC       AC_BRIGHTNESS_INC
#define GC_BRIGHTNESS_DEC       AC_BRIGHTNESS_DEC

/* Mousekey */
#define GC_MS_UP                AC_MS_UP
#define GC_MS_DOWN              AC_MS_DOWN
#define GC_MS_LEFT              AC_MS_LEFT
#define GC_MS_RIGHT             AC_MS_RIGHT
#define GC_MS_BTN1              AC_MS_BTN1
#define GC_MS_BTN2              AC_MS_BTN2
#define GC_MS_BTN3              AC_MS_BTN3
#define GC_MS_BTN4              AC_MS_BTN4
#define GC_MS_BTN5              AC_MS_BTN5
#define GC_MS_WH_UP             AC_MS_WH_UP
#define GC_MS_WH_DOWN           AC_MS_WH_DOWN
#define GC_MS_WH_LEFT           AC_MS_WH_LEFT
#define GC_MS_WH_RIGHT          AC_MS_WH_RIGHT
#define GC_MS_ACCEL0            AC_MS_ACCEL0
#define GC_MS_ACCEL1            AC_MS_ACCEL1
#define GC_MS_ACCEL2            AC_MS_ACCEL2

/* Command */
#define GC_BOOTLOADER           AC_BOOTLOADER

/*
 * Short names
 */
#define GC_LCTL                 AC_LCTL
#define GC_RCTL                 AC_RCTL
#define GC_LSFT                 AC_LSFT
#define GC_RSFT                 AC_RSFT
#define GC_ESC                  AC_ESC
#define GC_BSPC                 AC_BSPC
#define GC_ENT                  AC_ENT
#define GC_DEL                  AC_DEL
#define GC_INS                  AC_INS
#define GC_CAPS                 AC_CAPS
#define GC_CLCK                 AC_CLCK
#define GC_RGHT                 AC_RGHT
#define GC_PGDN                 AC_PGDN
#define GC_PSCR                 AC_PSCR
#define GC_SLCK                 AC_SLCK
#define GC_PAUS                 AC_PAUS
#define GC_BRK                  AC_BRK
#define GC_NLCK                 AC_NLCK
#define GC_SPC                  AC_SPC
#define GC_MINS                 AC_MINS
#define GC_EQL                  AC_EQL
#define GC_GRV                  AC_GRV
#define GC_RBRC                 AC_RBRC
#define GC_LBRC                 AC_LBRC
#define GC_COMM                 AC_COMM
#define GC_BSLS                 AC_BSLS
#define GC_SLSH                 AC_SLSH
#define GC_SCLN                 AC_SCLN
#define GC_QUOT                 AC_QUOT
#define GC_APP                  AC_APP
#define GC_NUHS                 AC_NUHS
#define GC_NUBS                 AC_NUBS
#define GC_LCAP                 AC_LCAP
#define GC_LNUM                 AC_LNUM
#define GC_LSCR                 AC_LSCR
#define GC_ERAS                 AC_ERAS
#define GC_CLR                  AC_CLR

/* Japanese specific */
#define GC_ZKHK                 AC_ZKHK
#define GC_RO                   AC_RO
#define GC_KANA                 AC_KANA
#define GC_JYEN                 AC_JYEN
#define GC_HENK                 AC_HENK
#define GC_MHEN                 AC_MHEN

/* Keypad */
#define GC_P1                   AC_P1
#define GC_P2                   AC_P2
#define GC_P3                   AC_P3
#define GC_P4                   AC_P4
#define GC_P5                   AC_P5
#define GC_P6                   AC_P6
#define GC_P7                   AC_P7
#define GC_P8                   AC_P8
#define GC_P9                   AC_P9
#define GC_P0                   AC_P0
#define GC_P00                  AC_P00
#define GC_P000                 AC_P000
#define GC_PDOT                 AC_PDOT
#define GC_PCMM                 AC_PCMM
#define GC_PSLS                 AC_PSLS
#define GC_PAST                 AC_PAST
#define GC_PMNS                 AC_PMNS
#define GC_PPLS                 AC_PPLS
#define GC_PEQL                 AC_PEQL
#define GC_PENT                 AC_PENT

/* Mousekey */
#define GC_MS_U                 AC_MS_U
#define GC_MS_D                 AC_MS_D
#define GC_MS_L                 AC_MS_L
#define GC_MS_R                 AC_MS_R
#define GC_BTN1                 AC_BTN1
#define GC_BTN2                 AC_BTN2
#define GC_BTN3                 AC_BTN3
#define GC_BTN4                 AC_BTN4
#define GC_BTN5                 AC_BTN5
#define GC_WH_U                 AC_WH_U
#define GC_WH_D                 AC_WH_D
#define GC_WH_L                 AC_WH_L
#define GC_WH_R                 AC_WH_R
#define GC_ACL0                 AC_ACL0
#define GC_ACL1                 AC_ACL1
#define GC_ACL2                 AC_ACL2

/* System Control */
#define GC_PWR                  AC_PWR
#define GC_SLEP                 AC_SLEP
#define GC_WAKE                 AC_WAKE

/* Consumer Page */
#define GC_MUTE                 AC_MUTE
#define GC_VOLU                 AC_VOLU
#define GC_VOLD                 AC_VOLD
#define GC_MNXT                 AC_MNXT
#define GC_MPRV                 AC_MPRV
#define GC_MFFD                 AC_MFFD
#define GC_MRWD                 AC_MRWD
#define GC_MSTP                 AC_MSTP
#define GC_MPLY                 AC_MPLY
#define GC_EJCT                 AC_EJCT
#define GC_MSEL                 AC_MSEL
#define GC_MAIL                 AC_MAIL
#define GC_CALC                 AC_CALC
#define GC_MYCM                 AC_MYCM
#define GC_WSCH                 AC_WSCH
#define GC_WHOM                 AC_WHOM
#define GC_WBAK                 AC_WBAK
#define GC_WFWD                 AC_WFWD
#define GC_WSTP                 AC_WSTP
#define GC_WREF                 AC_WREF
#define GC_WFAV                 AC_WFAV
#define GC_BRTI                 AC_BRTI
#define GC_BRTD                 AC_BRTD

/* Transparent */
#define GC_TRNS                 AC_TRNS

/* Command */
#define GC_BTLD                 AC_BTLD

/* Misc */
#define GC_CTMB                 AC_CTMB
#define GC_MEH                  AC_MEH
#define GC_HYPER                AC_HYPER
#define GC_GESC                 AC_GESC
#define GC_SFTENT               AC_SFTENT
#define GC_CTME                 AC_CTME

#define GC_KEYMAPS              const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS]

#else
/* Normal key */
#define GC_NO                   KC_NO
#define GC_TRANSPARENT          KC_TRANSPARENT
#define GC_ROLL_OVER            KC_ROLL_OVER
#define GC_POST_FAIL            KC_POST_FAIL
#define GC_UNDEFINED            KC_UNDEFINED
#define GC_A                    KC_A
#define GC_B                    KC_B
#define GC_C                    KC_C
#define GC_D                    KC_D
#define GC_E                    KC_E
#define GC_F                    KC_F
#define GC_G                    KC_G
#define GC_H                    KC_H
#define GC_I                    KC_I
#define GC_J                    KC_J
#define GC_K                    KC_K
#define GC_L                    KC_L
#define GC_M                    KC_M
#define GC_N                    KC_N
#define GC_O                    KC_O
#define GC_P                    KC_P
#define GC_Q                    KC_Q
#define GC_R                    KC_R
#define GC_S                    KC_S
#define GC_T                    KC_T
#define GC_U                    KC_U
#define GC_V                    KC_V
#define GC_W                    KC_W
#define GC_X                    KC_X
#define GC_Y                    KC_Y
#define GC_Z                    KC_Z
#define GC_1                    KC_1
#define GC_2                    KC_2
#define GC_3                    KC_3
#define GC_4                    KC_4
#define GC_5                    KC_5
#define GC_6                    KC_6
#define GC_7                    KC_7
#define GC_8                    KC_8
#define GC_9                    KC_9
#define GC_0                    KC_0
#define GC_ENTER                KC_ENTER
#define GC_ESCAPE               KC_ESCAPE
#define GC_BSPACE               KC_BSPACE
#define GC_TAB                  KC_TAB
#define GC_SPACE                KC_SPACE
#define GC_MINUS                KC_MINUS
#define GC_EQUAL                KC_EQUAL
#define GC_LBRACKET             KC_LBRACKET
#define GC_RBRACKET             KC_RBRACKET
#define GC_BSLASH               KC_BSLASH
#define GC_NONUS_HASH           KC_NONUS_HASH
#define GC_SCOLON               KC_SCOLON
#define GC_QUOTE                KC_QUOTE
#define GC_GRAVE                KC_GRAVE
#define GC_COMMA                KC_COMMA
#define GC_DOT                  KC_DOT
#define GC_SLASH                KC_SLASH
#define GC_CAPSLOCK             KC_CAPSLOCK
#define GC_F1                   KC_F1
#define GC_F2                   KC_F2
#define GC_F3                   KC_F3
#define GC_F4                   KC_F4
#define GC_F5                   KC_F5
#define GC_F6                   KC_F6
#define GC_F7                   KC_F7
#define GC_F8                   KC_F8
#define GC_F9                   KC_F9
#define GC_F10                  KC_F10
#define GC_F11                  KC_F11
#define GC_F12                  KC_F12
#define GC_PSCREEN              KC_PSCREEN
#define GC_SCROLLLOCK           KC_SCROLLLOCK
#define GC_PAUSE                KC_PAUSE
#define GC_INSERT               KC_INSERT
#define GC_HOME                 KC_HOME
#define GC_PGUP                 KC_PGUP
#define GC_DELETE               KC_DELETE
#define GC_END                  KC_END
#define GC_PGDOWN               KC_PGDOWN
#define GC_RIGHT                KC_RIGHT
#define GC_LEFT                 KC_LEFT
#define GC_DOWN                 KC_DOWN
#define GC_UP                   KC_UP
#define GC_NUMLOCK              KC_NUMLOCK
#define GC_KP_SLASH             KC_KP_SLASH
#define GC_KP_ASTERISK          KC_KP_ASTERISK
#define GC_KP_MINUS             KC_KP_MINUS
#define GC_KP_PLUS              KC_KP_PLUS
#define GC_KP_ENTER             KC_KP_ENTER
#define GC_KP_1                 KC_KP_1
#define GC_KP_2                 KC_KP_2
#define GC_KP_3                 KC_KP_3
#define GC_KP_4                 KC_KP_4
#define GC_KP_5                 KC_KP_5
#define GC_KP_6                 KC_KP_6
#define GC_KP_7                 KC_KP_7
#define GC_KP_8                 KC_KP_8
#define GC_KP_9                 KC_KP_9
#define GC_KP_0                 KC_KP_0
#define GC_KP_DOT               KC_KP_DOT
#define GC_NONUS_BSLASH         KC_NONUS_BSLASH
#define GC_APPLICATION          KC_APPLICATION
#define GC_POWER                KC_POWER
#define GC_KP_EQUAL             KC_KP_EQUAL
#define GC_F13                  KC_F13
#define GC_F14                  KC_F14
#define GC_F15                  KC_F15
#define GC_F16                  KC_F16
#define GC_F17                  KC_F17
#define GC_F18                  KC_F18
#define GC_F19                  KC_F19
#define GC_F20                  KC_F20
#define GC_F21                  KC_F21
#define GC_F22                  KC_F22
#define GC_F23                  KC_F23
#define GC_F24                  KC_F24
#define GC_EXECUTE              KC_EXECUTE
#define GC_HELP                 KC_HELP
#define GC_MENU                 KC_MENU
#define GC_SELECT               KC_SELECT
#define GC_STOP                 KC_STOP
#define GC_AGAIN                KC_AGAIN
#define GC_UNDO                 KC_UNDO
#define GC_CUT                  KC_CUT
#define GC_COPY                 KC_COPY
#define GC_PASTE                KC_PASTE
#define GC_FIND                 KC_FIND
#define GC__MUTE                KC__MUTE
#define GC__VOLUP               KC__VOLUP
#define GC__VOLDOWN             KC__VOLDOWN
#define GC_LOCKING_CAPS         KC_LOCKING_CAPS
#define GC_LOCKING_NUM          KC_LOCKING_NUM
#define GC_LOCKING_SCROLL       KC_LOCKING_SCROLL
#define GC_KP_COMMA             KC_KP_COMMA
#define GC_KP_EQUAL_AS400       KC_KP_EQUAL_AS400
#define GC_INT1                 KC_INT1
#define GC_INT2                 KC_INT2
#define GC_INT3                 KC_INT3
#define GC_INT4                 KC_INT4
#define GC_INT5                 KC_INT5
#define GC_INT6                 KC_INT6
#define GC_INT7                 KC_INT7
#define GC_INT8                 KC_INT8
#define GC_INT9                 KC_INT9
#define GC_LANG1                KC_LANG1
#define GC_LANG2                KC_LANG2
#define GC_LANG3                KC_LANG3
#define GC_LANG4                KC_LANG4
#define GC_LANG5                KC_LANG5
#define GC_LANG6                KC_LANG6
#define GC_LANG7                KC_LANG7
#define GC_LANG8                KC_LANG8
#define GC_LANG9                KC_LANG9
#define GC_ALT_ERASE            KC_ALT_ERASE
#define GC_SYSREQ               KC_SYSREQ
#define GC_CANCEL               KC_CANCEL
#define GC_CLEAR                KC_CLEAR
#define GC_PRIOR                KC_PRIOR
#define GC_RETURN               KC_RETURN
#define GC_SEPARATOR            KC_SEPARATOR
#define GC_OUT                  KC_OUT
#define GC_OPER                 KC_OPER
#define GC_CLEAR_AGAIN          KC_CLEAR_AGAIN
#define GC_CRSEL                KC_CRSEL
#define GC_EXSEL                KC_EXSEL
#define GC_KP_00                KC_KP_00
#define GC_KP_000               KC_KP_000
#define GC_THOUSANDS_SEPARATOR  KC_THOUSANDS_SEPARATOR
#define GC_DECIMAL_SEPARATOR    KC_DECIMAL_SEPARATOR
#define GC_CURRENCY_UNIT        KC_CURRENCY_UNIT
#define GC_CURRENCY_SUB_UNIT    KC_CURRENCY_SUB_UNIT
#define GC_KP_LPAREN            KC_KP_LPAREN
#define GC_KP_RPAREN            KC_KP_RPAREN
#define GC_KP_LCBRACKET         KC_KP_LCBRACKET
#define GC_KP_RCBRACKET         KC_KP_RCBRACKET
#define GC_KP_TAB               KC_KP_TAB
#define GC_KP_BSPACE            KC_KP_BSPACE
#define GC_KP_A                 KC_KP_A
#define GC_KP_B                 KC_KP_B
#define GC_KP_C                 KC_KP_C
#define GC_KP_D                 KC_KP_D
#define GC_KP_E                 KC_KP_E
#define GC_KP_F                 KC_KP_F
#define GC_KP_XOR               KC_KP_XOR
#define GC_KP_HAT               KC_KP_HAT
#define GC_KP_PERC              KC_KP_PERC
#define GC_KP_LT                KC_KP_LT
#define GC_KP_GT                KC_KP_GT
#define GC_KP_AND               KC_KP_AND
#define GC_KP_LAZYAND           KC_KP_LAZYAND
#define GC_KP_OR                KC_KP_OR
#define GC_KP_LAZYOR            KC_KP_LAZYOR
#define GC_KP_COLON             KC_KP_COLON
#define GC_KP_HASH              KC_KP_HASH
#define GC_KP_SPACE             KC_KP_SPACE
#define GC_KP_ATMARK            KC_KP_ATMARK
#define GC_KP_EXCLAMATION       KC_KP_EXCLAMATION
#define GC_KP_MEM_STORE         KC_KP_MEM_STORE
#define GC_KP_MEM_RECALL        KC_KP_MEM_RECALL
#define GC_KP_MEM_CLEAR         KC_KP_MEM_CLEAR
#define GC_KP_MEM_ADD           KC_KP_MEM_ADD
#define GC_KP_MEM_SUB           KC_KP_MEM_SUB
#define GC_KP_MEM_MUL           KC_KP_MEM_MUL
#define GC_KP_MEM_DIV           KC_KP_MEM_DIV
#define GC_KP_PLUS_MINUS        KC_KP_PLUS_MINUS
#define GC_KP_CLEAR             KC_KP_CLEAR
#define GC_KP_CLEAR_ENTRY       KC_KP_CLEAR_ENTRY
#define GC_KP_BINARY            KC_KP_BINARY
#define GC_KP_OCTAL             KC_KP_OCTAL
#define GC_KP_DECIMAL           KC_KP_DECIMAL
#define GC_KP_HEXADECIMAL       KC_KP_HEXADECIMAL

/* Modifiers */
#define GC_LCTRL                KC_LCTRL
#define GC_LSHIFT               KC_LSHIFT
#define GC_LALT                 KC_LALT
#define GC_LGUI                 KC_LGUI
#define GC_RCTRL                KC_RCTRL
#define GC_RSHIFT               KC_RSHIFT
#define GC_RALT                 KC_RALT
#define GC_RGUI                 KC_RGUI

/* Sytem Control */
#define GC_SYSTEM_POWER         KC_SYSTEM_POWER
#define GC_SYSTEM_SLEEP         KC_SYSTEM_SLEEP
#define GC_SYSTEM_WAKE          KC_SYSTEM_WAKE

/* Consumer Page */
#define GC_AUDIO_MUTE           KC_AUDIO_MUTE
#define GC_AUDIO_VOL_UP         KC_AUDIO_VOL_UP
#define GC_AUDIO_VOL_DOWN       KC_AUDIO_VOL_DOWN
#define GC_MEDIA_NEXT_TRACK     KC_MEDIA_NEXT_TRACK
#define GC_MEDIA_PREV_TRACK     KC_MEDIA_PREV_TRACK
#define GC_MEDIA_FAST_FORWARD   KC_MEDIA_FAST_FORWARD
#define GC_MEDIA_REWIND         KC_MEDIA_REWIND
#define GC_MEDIA_STOP           KC_MEDIA_STOP
#define GC_PLAY_PAUSE           KC_MEDIA_PLAY_PAUSE
#define GC_MEDIA_EJECT          KC_MEDIA_EJECT
#define GC_MEDIA_SELECT         KC_MEDIA_SELECT
#define GC_MAIL                 KC_MAIL
#define GC_CALCULATOR           KC_CALCULATOR
#define GC_MY_COMPUTER          KC_MY_COMPUTER
#define GC_WWW_SEARCH           KC_WWW_SEARCH
#define GC_WWW_HOME             KC_WWW_HOME
#define GC_WWW_BACK             KC_WWW_BACK
#define GC_WWW_FORWARD          KC_WWW_FORWARD
#define GC_WWW_STOP             KC_WWW_STOP
#define GC_WWW_REFRESH          KC_WWW_REFRESH
#define GC_WWW_FAVORITES        KC_WWW_FAVORITES
#define GC_BRIGHTNESS_INC       KC_BRIGHTNESS_INC
#define GC_BRIGHTNESS_DEC       KC_BRIGHTNESS_DEC

/* Mousekey */
#define GC_MS_UP                KC_MS_UP
#define GC_MS_DOWN              KC_MS_DOWN
#define GC_MS_LEFT              KC_MS_LEFT
#define GC_MS_RIGHT             KC_MS_RIGHT
#define GC_MS_BTN1              KC_MS_BTN1
#define GC_MS_BTN2              KC_MS_BTN2
#define GC_MS_BTN3              KC_MS_BTN3
#define GC_MS_BTN4              KC_MS_BTN4
#define GC_MS_BTN5              KC_MS_BTN5
#define GC_MS_WH_UP             KC_MS_WH_UP
#define GC_MS_WH_DOWN           KC_MS_WH_DOWN
#define GC_MS_WH_LEFT           KC_MS_WH_LEFT
#define GC_MS_WH_RIGHT          KC_MS_WH_RIGHT
#define GC_MS_ACCEL0            KC_MS_ACCEL0
#define GC_MS_ACCEL1            KC_MS_ACCEL1
#define GC_MS_ACCEL2            KC_MS_ACCEL2

/* Command */
#define GC_BOOTLOADER           KC_BOOTLOADER

/*
 * Short names
 */
#define GC_LCTL                 KC_LCTL
#define GC_RCTL                 KC_RCTL
#define GC_LSFT                 KC_LSFT
#define GC_RSFT                 KC_RSFT
#define GC_ESC                  KC_ESC
#define GC_BSPC                 KC_BSPC
#define GC_ENT                  KC_ENT
#define GC_DEL                  KC_DEL
#define GC_INS                  KC_INS
#define GC_CAPS                 KC_CAPS
#define GC_CLCK                 KC_CLCK
#define GC_RGHT                 KC_RGHT
#define GC_PGDN                 KC_PGDN
#define GC_PSCR                 KC_PSCR
#define GC_SLCK                 KC_SLCK
#define GC_PAUS                 KC_PAUS
#define GC_BRK                  KC_BRK
#define GC_NLCK                 KC_NLCK
#define GC_SPC                  KC_SPC
#define GC_MINS                 KC_MINS
#define GC_EQL                  KC_EQL
#define GC_GRV                  KC_GRV
#define GC_RBRC                 KC_RBRC
#define GC_LBRC                 KC_LBRC
#define GC_COMM                 KC_COMM
#define GC_BSLS                 KC_BSLS
#define GC_SLSH                 KC_SLSH
#define GC_SCLN                 KC_SCLN
#define GC_QUOT                 KC_QUOT
#define GC_APP                  KC_APP
#define GC_NUHS                 KC_NUHS
#define GC_NUBS                 KC_NUBS
#define GC_LCAP                 KC_LCAP
#define GC_LNUM                 KC_LNUM
#define GC_LSCR                 KC_LSCR
#define GC_ERAS                 KC_ERAS
#define GC_CLR                  KC_CLR

/* Japanese specific */
#define GC_ZKHK                 KC_ZKHK
#define GC_RO                   KC_RO
#define GC_KANA                 KC_KANA
#define GC_JYEN                 KC_JYEN
#define GC_HENK                 KC_HENK
#define GC_MHEN                 KC_MHEN

/* Keypad */
#define GC_P1                   KC_P1
#define GC_P2                   KC_P2
#define GC_P3                   KC_P3
#define GC_P4                   KC_P4
#define GC_P5                   KC_P5
#define GC_P6                   KC_P6
#define GC_P7                   KC_P7
#define GC_P8                   KC_P8
#define GC_P9                   KC_P9
#define GC_P0                   KC_P0
#define GC_P00                  KC_P00
#define GC_P000                 KC_P000
#define GC_PDOT                 KC_PDOT
#define GC_PCMM                 KC_PCMM
#define GC_PSLS                 KC_PSLS
#define GC_PAST                 KC_PAST
#define GC_PMNS                 KC_PMNS
#define GC_PPLS                 KC_PPLS
#define GC_PEQL                 KC_PEQL
#define GC_PENT                 KC_PENT

/* Mousekey */
#define GC_MS_U                 KC_MS_U
#define GC_MS_D                 KC_MS_D
#define GC_MS_L                 KC_MS_L
#define GC_MS_R                 KC_MS_R
#define GC_BTN1                 KC_BTN1
#define GC_BTN2                 KC_BTN2
#define GC_BTN3                 KC_BTN3
#define GC_BTN4                 KC_BTN4
#define GC_BTN5                 KC_BTN5
#define GC_WH_U                 KC_WH_U
#define GC_WH_D                 KC_WH_D
#define GC_WH_L                 KC_WH_L
#define GC_WH_R                 KC_WH_R
#define GC_ACL0                 KC_ACL0
#define GC_ACL1                 KC_ACL1
#define GC_ACL2                 KC_ACL2

/* System Control */
#define GC_PWR                  KC_PWR
#define GC_SLEP                 KC_SLEP
#define GC_WAKE                 KC_WAKE

/* Consumer Page */
#define GC_MUTE                 KC_MUTE
#define GC_VOLU                 KC_VOLU
#define GC_VOLD                 KC_VOLD
#define GC_MNXT                 KC_MNXT
#define GC_MPRV                 KC_MPRV
#define GC_MFFD                 KC_MFFD
#define GC_MRWD                 KC_MRWD
#define GC_MSTP                 KC_MSTP
#define GC_MPLY                 KC_MPLY
#define GC_EJCT                 KC_EJCT
#define GC_MSEL                 KC_MSEL
#define GC_MAIL                 KC_MAIL
#define GC_CALC                 KC_CALC
#define GC_MYCM                 KC_MYCM
#define GC_WSCH                 KC_WSCH
#define GC_WHOM                 KC_WHOM
#define GC_WBAK                 KC_WBAK
#define GC_WFWD                 KC_WFWD
#define GC_WSTP                 KC_WSTP
#define GC_WREF                 KC_WREF
#define GC_WFAV                 KC_WFAV
#define GC_BRTI                 KC_BRTI
#define GC_BRTD                 KC_BRTD

/* Transparent */
#define GC_TRNS                 KC_TRNS

/* Command */
#define GC_BTLD                 KC_BTLD

/* Misc */
#define GC_CTMB                 KC_CTMB
#define GC_MEH                  KC_MEH
#define GC_HYPER                KC_HYPER
#define GC_GESC                 KC_GESC
#define GC_SFTENT               KC_SFTENT
#define GC_CTME                 KC_CTME
#define GC_KEYMAPS              const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS]

/* FN */
#define GC_FN0                  KC_FN0
#define GC_FN1                  KC_FN1
#define GC_FN2                  KC_FN2
#define GC_FN3                  KC_FN3
#define GC_FN4                  KC_FN4
#define GC_FN5                  KC_FN5
#define GC_FN6                  KC_FN6
#define GC_FN7                  KC_FN7
#define GC_FN8                  KC_FN8
#define GC_FN9                  KC_FN9
#define GC_FN10                 KC_FN10
#define GC_FN11                 KC_FN11
#define GC_FN12                 KC_FN12
#define GC_FN13                 KC_FN13
#define GC_FN14                 KC_FN14
#define GC_FN15                 KC_FN15
#define GC_FN16                 KC_FN16
#define GC_FN17                 KC_FN17
#define GC_FN18                 KC_FN18
#define GC_FN19                 KC_FN19
#define GC_FN20                 KC_FN20
#define GC_FN21                 KC_FN21
#define GC_FN22                 KC_FN22
#define GC_FN23                 KC_FN23
#define GC_FN24                 KC_FN24
#define GC_FN25                 KC_FN25
#define GC_FN26                 KC_FN26
#define GC_FN27                 KC_FN27
#define GC_FN28                 KC_FN28
#define GC_FN29                 KC_FN29
#define GC_FN30                 KC_FN30
#define GC_FN31                 KC_FN31
#endif
