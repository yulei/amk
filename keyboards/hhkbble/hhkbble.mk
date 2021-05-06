
SRCS += $(KEYBOARD_DIR)/hhkbble.c

MCU = NRF52832
RGB_EFFECTS_ENABLE = all
#EECONFIG_FRAM = yes
KEYMAP_CONFIG_ENABLE = yes
#EECONFIG_FDS = yes
#NRF_AMK_DEBUG = yes
ACTIONMAP_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/hhkbble_action.c
else
	SRCS += $(KEYBOARD_DIR)/hhkbble_keymap.c
endif