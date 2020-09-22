RFHOST_DIR := $(TOP_DIR)/keyboards/rfhost

SRC_FILES += \
	$(RFHOST_DIR)/rfhost.c \
	$(RFHOST_DIR)/rfhost_keymap.c \

INC_FOLDERS += \
	$(RFHOST_DIR) \

MCU = NRF52840
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
CUSTOM_MATRIX = yes

SRC_FILES += \
	$(RFHOST_DIR)/rfhost_matrix.c \
