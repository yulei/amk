
SRCS += $(KEYBOARD_DIR)/hhkbble.c

MCU = NRF52832
RGB_EFFECTS_ENABLE = all
EECONFIG_FDS = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_action.c)
else
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_keymap.c)
endif