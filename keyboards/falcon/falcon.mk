
SRCS += $(KEYBOARD_DIR)/falcon.c

MCU = NRF52832
EECONFIG_FLASH = yes
RGB_LINEAR_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_action.c)
else
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_keymap.c)
endif