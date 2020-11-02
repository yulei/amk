
KEYBOARD_DIR := $(TOP_DIR)/keyboards/usbmaster

SRCS += \
	$(KEYBOARD_DIR)/usbmaster.c \
	$(KEYBOARD_DIR)/usbmaster_keymap.c \
	$(KEYBOARD_DIR)/usbmaster_matrix.c

INCS += \
	$(KEYBOARD_DIR) \

MCU = STM32F103
BOOTMAGIC_ENABLE = yes
CUSTOM_MATRIX = yes
WEBUSB_ENABLE = yes
