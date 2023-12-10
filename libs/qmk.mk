
QMK_PORT_DIR := $(LIB_DIR)/qmk/portable
QMK_DIR := $(LIB_DIR)/qmk/vial-qmk

include $(LIB_DIR)/qmk/qmk_feature.mk

SRCS += \
	$(QMK_PORT_DIR)/qmk_driver.c \

SRCS += \
	$(LIB_DIR)/qmk/protocol/host.c \
	$(LIB_DIR)/qmk/protocol/report.c \
	$(LIB_DIR)/qmk/protocol/keyboard.c \

SRCS += \
    $(QMK_DIR)/quantum/quantum.c \
    $(QMK_DIR)/quantum/bitwise.c \
    $(QMK_DIR)/quantum/led.c \
    $(QMK_DIR)/quantum/action.c \
    $(QMK_DIR)/quantum/action_layer.c \
    $(QMK_DIR)/quantum/action_tapping.c \
    $(QMK_DIR)/quantum/action_util.c \
    $(QMK_DIR)/quantum/eeconfig.c \
    $(QMK_DIR)/quantum/matrix_common.c \
    $(QMK_DIR)/quantum/keymap_common.c \
    $(QMK_DIR)/quantum/keymap_introspection.c \
    $(QMK_DIR)/quantum/keycode_config.c \
    $(QMK_DIR)/quantum/sync_timer.c \
    $(QMK_DIR)/quantum/bootmagic/bootmagic_lite.c \
    $(QMK_DIR)/quantum/bootmagic/magic.c \
    $(QMK_DIR)/quantum/logging/debug.c \
    $(QMK_DIR)/quantum/logging/print.c \
    $(QMK_DIR)/quantum/logging/sendchar.c \

INCS += \
	$(QMK_DIR) \
	$(QMK_DIR)/platforms \
	$(QMK_DIR)/quantum \
	$(QMK_DIR)/quantum/audio \
	$(QMK_DIR)/quantum/bootmagic \
	$(QMK_DIR)/quantum/logging \
	$(QMK_DIR)/quantum/sequencer \
	$(QMK_DIR)/quantum/keymap_extras \

INCS += \
	$(QMK_PORT_DIR) \
	$(LIB_DIR)/qmk/protocol \

APP_DEFS += \
	-include config.h \
	-DIGNORE_ATOMIC_BLOCK \
    -DBOOTMAGIC_LITE \
	-DEEPROM_CUSTOM \
	-DEEPROM_SIZE=2000

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
	SRCS += $(LIB_DIR)/qmk/protocol/dynamic_keymap_amk.c
	SRCS += $(LIB_DIR)/qmk/protocol/send_string.c

    APP_DEFS += -DDYNAMIC_KEYMAP_ENABLE
    SRCS += $(QMK_DIR)/quantum/via.c
    APP_DEFS += -DVIA_ENABLE
    APP_DEFS += -DRAW_ENABLE

    VIAL_INSECURE ?= yes
    QMK_SETTINGS ?= no
    ifeq ($(strip $(TAP_DANCE_ENABLE)), yes)
        APP_DEFS += -DTAPPING_TERM_PER_KEY
    endif
    COMBO_ENABLE ?= yes
    KEY_OVERRIDE_ENABLE ?= no
    SRCS += $(QMK_DIR)/quantum/vial.c
    APP_DEFS += -DVIAL_ENABLE
    APP_DEFS += -DNO_DEBUG
    APP_DEFS += -DSEND_STRING_ENABLE
    INCS += $(QMK_DIR)/quantum/send_string
endif

ifeq ($(strip $(VIAL_INSECURE)), yes)
    APP_DEFS += -DVIAL_INSECURE
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

ifeq (yes, $(strip $(VIAL_ENABLE)))
$(QMK_DIR)/quantum/vial.c: $(KEYBOARD_DIR)/vial_generated_keyboard_definition.h

$(KEYBOARD_DIR)/vial_generated_keyboard_definition.h: $(KEYBOARD_DIR)/vial.json
	@python3 $(QMK_DIR)/util/vial_generate_definition.py $(KEYBOARD_DIR)/vial.json $(KEYBOARD_DIR)/vial_generated_keyboard_definition.h

$(QMK_DIR)/quantum/via.c: $(OUTPUT_DIR)/version.h
INCS += $(OUTPUT_DIR)

$(OUTPUT_DIR)/version.h: FORCE
	@python3 $(LIB_DIR)/qmk/generate_version.py $(QMK_DIR) $(OUTPUT_DIR)/version.h

FORCE:

endif

INCS += $(QMK_DIR)/lib/lib8tion