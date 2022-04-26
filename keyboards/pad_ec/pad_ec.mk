
SRCS += \
	$(KEYBOARD_DIR)/custom_matrix.c \
	$(KEYBOARD_DIR)/pad_ec.c \

MCU = STM32F446
USB_HOST_ENABLE = yes
EECONFIG_FLASH = yes
#VIAL_ENABLE = yes

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

LINKER_PATH = $(KEYBOARD_DIR)

ifeq (yes, $(strip $(VIAL_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/pad_ec_actionmap.c
else
	SRCS += $(KEYBOARD_DIR)/pad_ec_keymap.c
endif
