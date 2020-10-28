
KBD_DIR := $(TOP_DIR)/keyboards/pad

SRC_FILES += \
	$(KBD_DIR)/pad.c \
	$(KBD_DIR)/pad_keymap.c \
	$(KBD_DIR)/pad_matrix.c \

INC_FOLDERS += \
	$(KBD_DIR) \

MCU = STM32F405 
BOOTMAGIC_ENABLE = yes
CUSTOM_MATRIX = yes