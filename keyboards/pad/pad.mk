
SRCS += $(wildcard $(KEYBOARD_DIR)/*.c)

MCU = STM32F405 
USB_HOST_ENABLE = yes
RGB_MATRIX_ENABLE = yes
RGB_LINEAR_ENABLE = yes
EECONFIG_FLASH = yes

LINKER_PATH = $(KEYBOARD_DIR)