
TMK_DIR := $(LIB_DIR)/tmk/tmk_core

SRCS += \
	$(TMK_DIR)/common/host.c \
	$(TMK_DIR)/common/keyboard.c \
	$(TMK_DIR)/common/matrix.c \
	$(TMK_DIR)/common/action.c \
	$(TMK_DIR)/common/action_tapping.c  \
	$(TMK_DIR)/common/action_macro.c \
	$(TMK_DIR)/common/action_layer.c \
	$(TMK_DIR)/common/action_util.c  \
	$(TMK_DIR)/common/debug.c \
	$(TMK_DIR)/common/util.c  \
	$(TMK_DIR)/common/hook.c \

INCS += \
	$(TMK_DIR)/common \

APP_DEFS += \
	-include config.h \

ifeq (yes,$(strip $(MOUSEKEY_ENABLE)))
    SRCS += $(TMK_DIR)/common/mousekey.c
    APP_DEFS += -DMOUSEKEY_ENABLE
    APP_DEFS += -DMOUSE_ENABLE
endif

ifeq (yes,$(strip $(EXTRAKEY_ENABLE)))
    APP_DEFS += -DEXTRAKEY_ENABLE
endif
    
ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(TMK_DIR)/common/actionmap.c
	APP_DEFS += -DACTIONMAP_ENABLE
else
	SRCS += $(TMK_DIR)/common/keymap.c
endif