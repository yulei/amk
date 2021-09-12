
SRCS += \
	$(KEYBOARD_DIR)/usbmaster.c \
	$(KEYBOARD_DIR)/ring_buffer.c \
	$(KEYBOARD_DIR)/display.c \


#APP_DEFS += -DABELX
#APP_DEFS += -DADD
#APP_DEFS += -DFALCON
APP_DEFS += -DVITA

#MCU = STM32F103
#MCU = STM32L072
MCU = STM32F411
CUSTOM_MATRIX = yes
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
#VIAL_ENABLE = no 
ACTIONMAP_ENABLE = yes
#DATETIME_ENABLE = yes
#RTC_DRIVER = bl5372
SCREEN_ENABLE = yes
MSC_ENABLE = yes
#RTOS_ENABLE = yes

LINKER_PATH = $(KEYBOARD_DIR)