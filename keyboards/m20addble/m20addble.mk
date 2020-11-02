
M20ADDBLE_DIR := $(TOP_DIR)/keyboards/m20addble

SRCS += \
	$(M20ADDBLE_DIR)/m20addble.c \
	$(M20ADDBLE_DIR)/m20addble_keymap.c \

INCS += \
	$(M20ADDBLE_DIR) \

MCU = NRF52840
BOOTMAGIC_ENABLE = yes
MATRIX_USE_TCA6424 = yes