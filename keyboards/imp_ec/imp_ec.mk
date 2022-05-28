
MCU = STM32F411
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
RGB_LINEAR_ENABLE = yes
EECONFIG_FLASH = yes
VIAL_ENABLE = yes
MATRIX_EC_ENABLE = yes

LINKER_PATH = $(KEYBOARD_DIR)

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

#HID_OTHER_ENABLE = no
#CDC_ENABLE = yes

SRCS += \
	$(KEYBOARD_DIR)/imp_ec.c \
#	$(KEYBOARD_DIR)/custom_matrix.c \

ifeq (yes, $(strip $(VIAL_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/imp_ec_actionmap.c
else
	SRCS += $(KEYBOARD_DIR)/imp_ec_keymap.c
endif