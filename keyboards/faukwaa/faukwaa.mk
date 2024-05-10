MCU = STM32F405
USE_F405_APM = yes

TINYUSB_ENABLE = yes
RGB_LINEAR_ENABLE = yes
RGB_MATRIX_ENABLE = yes
CUSTOMRGB_ENABLE = yes
RGB_INDICATOR_ENABLE = yes
EECONFIG_FLASH = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
PORT_SCAN_ENABLE = yes
STATE_SCAN_ENABLE = yes
DEBOUNCE_TYPE = none

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

LINKER_PATH = $(KEYBOARD_DIR)

SRCS += $(KEYBOARD_DIR)/faukwaa.c
