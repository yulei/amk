
QMK_LIB_DIR := $(LIB_DIR)/qmk
QMK_DIR := $(LIB_DIR)/qmk/vial-qmk

SRCS += \
	$(QMK_LIB_DIR)/qmk_driver.c \
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

ifneq (yes,$(strip $(NKRO_AUTO_ENABLE)))
	SRCS += $(QMK_DIR)/quantum/action_util.c
endif

ifeq (yes,$(strip $(MOUSEKEY_ENABLE)))
    SRCS += $(QMK_DIR)/quantum/mousekey.c
    APP_DEFS += -DMOUSEKEY_ENABLE
    APP_DEFS += -DMOUSE_ENABLE
endif

ifeq (yes,$(strip $(EXTRAKEY_ENABLE)))
    APP_DEFS += -DEXTRAKEY_ENABLE
endif
