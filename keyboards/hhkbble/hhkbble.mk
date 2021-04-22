
SRCS += $(KEYBOARD_DIR)/hhkbble.c

MCU = NRF52832
RGB_EFFECTS_ENABLE = all

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/hhkbble_action.c
else
	SRCS += $(KEYBOARD_DIR)/hhkbble_keymap.c
endif