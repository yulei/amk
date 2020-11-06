
SRCS += \
	$(KEYBOARD_DIR)/hhkbble.c \

INCS += \
	$(KEYBOARD_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
RGB_EFFECTS_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/hhkbble_action.c
else
	SRCS += $(KEYBOARD_DIR)/hhkbble_keymap.c
endif