
SRCS += $(KEYBOARD_DIR)/m65ble.c

MCU = NRF52832
EECONFIG_FRAM = yes 
RGB_LINEAR_ENABLE = yes
RGB_INDICATOR_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_action.c)
else
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_keymap.c)
endif
