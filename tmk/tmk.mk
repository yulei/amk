
TMK_DIR := $(TOP_DIR)/tmk

SRC_FILES += \
	$(TMK_DIR)/tmk/tmk_core/common/host.c \
	$(TMK_DIR)/tmk/tmk_core/common/keyboard.c \
	$(TMK_DIR)/tmk/tmk_core/common/matrix.c \
	$(TMK_DIR)/tmk/tmk_core/common/action.c \
	$(TMK_DIR)/tmk/tmk_core/common/action_tapping.c  \
	$(TMK_DIR)/tmk/tmk_core/common/action_macro.c \
	$(TMK_DIR)/tmk/tmk_core/common/action_layer.c \
	$(TMK_DIR)/tmk/tmk_core/common/action_util.c  \
	$(TMK_DIR)/tmk/tmk_core/common/print.c  \
	$(TMK_DIR)/tmk/tmk_core/common/debug.c \
	$(TMK_DIR)/tmk/tmk_core/common/util.c  \
	$(TMK_DIR)/tmk/tmk_core/common/hook.c \
	$(TMK_DIR)/tmk/tmk_core/common/keymap.c \
	$(TMK_DIR)/tmk/tmk_core/common/bootmagic.c \
	$(TMK_DIR)/tmk/tmk_core/common/mousekey.c \

INC_FOLDERS += \
	$(TMK_DIR) \
	$(TMK_DIR)/tmk/tmk_core/common \

TMK_DEFS += -include config.h
TMK_DEFS += -DBOOTMAGIC_ENABLE
TMK_DEFS += -DMOUSEKEY_ENABLE
TMK_DEFS += -DEXTRAKEY_ENABLE