
MCU = STM32F411
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
RGB_LINEAR_ENABLE = yes
#EECONFIG_FLASH = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

LINKER_PATH = $(KEYBOARD_DIR)

SRCS += $(KEYBOARD_DIR)/8xv2.c