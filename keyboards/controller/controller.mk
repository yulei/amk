
CONTROLLER_DIR := $(TOP_DIR)/keyboards/controller

SRC_FILES += \
	$(CONTROLLER_DIR)/controller.c \
	$(CONTROLLER_DIR)/controller_keymap.c \

INC_FOLDERS += \
	$(CONTROLLER_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
CUSTOM_MATRIX = yes

SRC_FILES += \
	$(CONTROLLER_DIR)/controller_matrix.c \
