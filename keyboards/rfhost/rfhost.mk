RFHOST_DIR := $(TOP_DIR)/keyboards/rfhost

SRCS += \
	$(RFHOST_DIR)/rfhost.c \
	$(RFHOST_DIR)/rfhost_keymap.c \
	$(RFHOST_DIR)/rfhost_matrix.c \

INCS += \
	$(RFHOST_DIR) \

MCU = NRF52840
BOOTMAGIC_ENABLE = yes
CUSTOM_MATRIX = yes
