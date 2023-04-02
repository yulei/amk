
QMK_LIB_DIR := $(LIB_DIR)/qmk
QMK_DIR := $(LIB_DIR)/qmk/vial-qmk

SRCS += \
	$(QMK_LIB_DIR)/qmk_driver.c \

SRCS += \
    $(QMK_DIR)/quantum/quantum.c \
    $(QMK_DIR)/quantum/bitwise.c \
    $(QMK_DIR)/quantum/led.c \
    $(QMK_DIR)/quantum/action.c \
    $(QMK_DIR)/quantum/action_layer.c \
    $(QMK_DIR)/quantum/action_tapping.c \
    $(QMK_DIR)/quantum/action_util.c \
    $(QMK_DIR)/quantum/eeconfig.c \
    $(QMK_DIR)/quantum/keyboard.c \
    $(QMK_DIR)/quantum/matrix_common.c \
    $(QMK_DIR)/quantum/keymap_common.c \
    $(QMK_DIR)/quantum/keycode_config.c \
    $(QMK_DIR)/quantum/sync_timer.c \
    $(QMK_DIR)/quantum/logging/debug.c \
    $(QMK_DIR)/quantum/logging/print.c \
    $(QMK_DIR)/quantum/logging/sendchar.c \
    $(QMK_DIR)/quantum/bootmagic/bootmagic_lite.c \
    $(QMK_DIR)/quantum/bootmagic/magic.c \
    $(QMK_DIR)/quantum/debounce/sym_defer_g.c \
    $(QMK_DIR)/quantum/mousekey.c \

SRCS += \
    $(QMK_DIR)/tmk_core/protocol/host.c \
    $(QMK_DIR)/tmk_core/protocol/report.c \


INCS += \
	$(QMK_DIR) \
	$(QMK_DIR)/platforms \
	$(QMK_DIR)/quantum \
	$(QMK_DIR)/quantum/audio \
	$(QMK_DIR)/quantum/bootmagic \
	$(QMK_DIR)/quantum/logging \
	$(QMK_DIR)/quantum/sequencer \
	$(QMK_DIR)/tmk_core/protocol \
	$(QMK_LIB_DIR) \

APP_DEFS += \
	-include config.h \
	-DIGNORE_ATOMIC_BLOCK \
    -DBOOTMAGIC_LITE \
	-DEEPROM_CUSTOM \
	-DEEPROM_SIZE=2048

MOUSEKEY_ENABLE := yes
EXTRAKEY_ENABLE := yes

ifeq (yes,$(strip $(MOUSEKEY_ENABLE)))
    SRCS += $(QMK_DIR)/quantum/mousekey.c
    APP_DEFS += -DMOUSEKEY_ENABLE
    APP_DEFS += -DMOUSE_ENABLE
endif

ifeq (yes,$(strip $(EXTRAKEY_ENABLE)))
    APP_DEFS += -DEXTRAKEY_ENABLE
endif

ifeq ($(strip $(VIAL_ENABLE)), yes)
    SRCS += $(QMK_DIR)/quantum/dynamic_keymap.c
    APP_DEFS += -DDYNAMIC_KEYMAP_ENABLE
    SRCS += $(QMK_DIR)/quantum/via.c
    APP_DEFS += -DVIA_ENABLE
    APP_DEFS += -DRAW_ENABLE

    VIAL_INSECURE ?= yes
    QMK_SETTINGS ?= no
    TAP_DANCE_ENABLE ?= no
    ifeq ($(strip $(TAP_DANCE_ENABLE)), yes)
        APP_DEFS += -DTAPPING_TERM_PER_KEY
    endif
    COMBO_ENABLE ?= no
    KEY_OVERRIDE_ENABLE ?= no
    SRCS += $(QMK_DIR)/quantum/vial.c
    APP_DEFS += -DVIAL_ENABLE
    APP_DEFS += -DNO_DEBUG
    APP_DEFS += -DSEND_STRING_ENABLE
    INCS += $(QMK_DIR)/quantum/send_string
    SRCS += $(QMK_DIR)/quantum/send_string/send_string.c
endif

ifeq ($(strip $(VIALRGB_ENABLE)), yes)
    SRCS += $(QMK_DIR)/quantum/vial_rgb.c
    APP_DEFS += -DVIALRGB_ENABLE
endif

ifeq ($(strip $(QMK_SETTINGS)), yes)
    AUTO_SHIFT_ENABLE := yes
    SRCS += $(QMK_DIR)/quantum/qmk_settings.c
    APP_DEFS += -DQMK_SETTINGS \
        -DAUTO_SHIFT_NO_SETUP -DAUTO_SHIFT_REPEAT_PER_KEY -DAUTO_SHIFT_NO_AUTO_REPEAT_PER_KEY \
        -DPERMISSIVE_HOLD_PER_KEY -DIGNORE_MOD_TAP_INTERRUPT_PER_KEY -DTAPPING_FORCE_HOLD_PER_KEY -DRETRO_TAPPING_PER_KEY \
        -DCOMBO_TERM_PER_COMBO
endif

ifeq ($(strip $(AUTO_SHIFT_ENABLE)), yes)
    SRCS += $(QMK_DIR)/quantum/process_keycode/process_auto_shift.c
    APP_DEFS += -DAUTO_SHIFT_ENABLE
    ifeq ($(strip $(AUTO_SHIFT_MODIFIERS)), yes)
        APP_DEFS += -DAUTO_SHIFT_MODIFIERS
    endif
endif