
SRCS += $(KEYBOARD_DIR)/m65ble.c

APP_DEFS += -DM65_BLE_V1

MCU = NRF52832
EECONFIG_FRAM = yes 
RGB_LINEAR_ENABLE = yes
RGB_INDICATOR_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_action.c)
else
	SRCS += $(wildcard $(KEYBOARD_DIR)/*_keymap.c)
endif
