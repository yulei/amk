
SRCS += \
	$(KEYBOARD_DIR)/controller.c \
	$(KEYBOARD_DIR)/controller_keymap.c \
	$(KEYBOARD_DIR)/controller_matrix.c \

INCS += \
	$(KEYBOARD_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
CUSTOM_MATRIX = yes
