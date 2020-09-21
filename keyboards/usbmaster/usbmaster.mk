
KEYBOARD_DIR := $(TOP_DIR)/keyboards/usbmaster

SRC_FILES += \
	$(KEYBOARD_DIR)/usbmaster.c \
	$(KEYBOARD_DIR)/usbmaster_keymap.c \

INC_FOLDERS += \
	$(KEYBOARD_DIR) \

MCU = STM32F103
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
CUSTOM_MATRIX = yes

SRC_FILES += \
	$(KEYBOARD_DIR)/usbmaster_matrix.c