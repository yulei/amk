
#SRCS += $(wildcard $(KEYBOARD_DIR)/*.c)

MCU = STM32F411
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
RGB_LINEAR_ENABLE = yes
EECONFIG_FLASH = yes
#VIAL_ENABLE = yes

LINKER_PATH = $(KEYBOARD_DIR)

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

SRCS += $(KEYBOARD_DIR)/falcon_ec.c
SRCS += $(KEYBOARD_DIR)/custom_matrix.c
SRCS += $(KEYBOARD_DIR)/falcon_ec_keymap.c
#SRCS += $(KEYBOARD_DIR)/falcon_ec_actionmap.c
