
#MCU = STM32F411
MCU = STM32F405
USE_F405_APM = yes

TINYUSB_ENABLE = yes
#TINYUSB_USE_HAL = yes
EECONFIG_FLASH = yes
MSC_ENABLE = yes
DYNAMIC_CONFIGURATION = yes
RGB_MATRIX_ENABLE = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
AMK_CUSTOM_MATRIX = ms
DEBOUNCE_TYPE = none

LINKER_PATH = $(KEYBOARD_DIR)

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

SRCS += $(KEYBOARD_DIR)/ms_corsa.c
SRCS += $(KEYBOARD_DIR)/apc_matrix.c

SRCS += $(KEYBOARD_DIR)/display.c
SRCS += $(MAIN_DIR)/drivers/st7735.c

#NO_DEBUG := 0