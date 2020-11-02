
CONTROLLER_DIR := $(TOP_DIR)/keyboards/controller

SRCS += \
	$(CONTROLLER_DIR)/controller.c \
	$(CONTROLLER_DIR)/controller_keymap.c \
	$(CONTROLLER_DIR)/controller_matrix.c \

INCS += \
	$(CONTROLLER_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
CUSTOM_MATRIX = yes
