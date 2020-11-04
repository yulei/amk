
HHKBBLE_DIR := $(TOP_DIR)/keyboards/hhkbble

SRCS += \
	$(HHKBBLE_DIR)/hhkbble.c \

INCS += \
	$(HHKBBLE_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
RGB_EFFECTS_ENABLE = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(HHKBBLE_DIR)/hhkbble_action.c
else
	SRCS += $(HHKBBLE_DIR)/hhkbble_keymap.c
endif