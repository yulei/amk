
MCU = STM32F411
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
RGB_LINEAR_ENABLE = yes
EECONFIG_FLASH = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
AMK_CUSTOM_MATRIX = ms

LINKER_PATH = $(KEYBOARD_DIR)

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

SRCS += $(KEYBOARD_DIR)/ms_falcon.c
SRCS += $(KEYBOARD_DIR)/apc_matrix.c
