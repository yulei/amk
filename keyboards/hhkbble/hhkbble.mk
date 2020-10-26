
HHKBBLE_DIR := $(TOP_DIR)/keyboards/hhkbble

SRC_FILES += \
	$(HHKBBLE_DIR)/hhkbble.c \

INC_FOLDERS += \
	$(HHKBBLE_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
RGB_EFFECTS_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRC_FILES += $(HHKBBLE_DIR)/hhkbble_action.c
else
	SRC_FILES += $(HHKBBLE_DIR)/hhkbble_keymap.c
endif