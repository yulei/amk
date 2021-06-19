
SRCS += $(KEYBOARD_DIR)/hhkbble.c

MCU = NRF52832
#KEYMAP_CONFIG_ENABLE = yes
#EECONFIG_FRAM = yes
EECONFIG_FLASH = yes
#NRF_AMK_DEBUG = yes
ACTIONMAP_ENABLE = yes
RGB_LINEAR_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/hhkbble_action.c
else
	SRCS += $(KEYBOARD_DIR)/hhkbble_keymap.c
endif