include $(MAIN_DIR)/porting/common_porting.mk

STM32_PORTING_DIR := $(MAIN_DIR)/porting/stm32

SRCS += \
	$(STM32_PORTING_DIR)/gpio_pin.c \
	$(STM32_PORTING_DIR)/stm32.c \
	$(STM32_PORTING_DIR)/wait.c \

INCS += \
	$(STM32_PORTING_DIR) \

ifneq (yes, $(TINYUSB_ENABLE))
	SRCS += $(STM32_PORTING_DIR)/usb_hal.c
endif

ifeq (yes, $(EECONFIG_FRAM)) 
	SRCS += $(MAIN_DIR)/eeconfig_fram.c
	SRCS += $(MAIN_DIR)/drivers/mb85rcxx.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DEECONFIG_FRAM
else
	ifeq (yes, $(EECONFIG_FLASH))
		SRCS += $(STM32_PORTING_DIR)/eeconfig_flash.c
		APP_DEFS += -DEECONFIG_FLASH
	else
		SRCS += $(MAIN_DIR)/eeconfig_mem.c
	endif
endif

ifeq (STM32F411, $(strip $(MCU)))
	SRCS += $(STM32_PORTING_DIR)/stm32f411xe.c
endif

ifeq (STM32F405, $(strip $(MCU)))
	SRCS += $(STM32_PORTING_DIR)/stm32f405xx.c
endif

ifeq (STM32F722, $(strip $(MCU)))
	SRCS += $(STM32_PORTING_DIR)/stm32f722xx.c
endif

ifeq (STM32F103, $(strip $(MCU)))
	SRCS += $(STM32_PORTING_DIR)/stm32f103xb.c
endif

ifeq (STM32L432, $(strip $(MCU)))
	SRCS += $(STM32_PORTING_DIR)/stm32l432xx.c
endif

ifeq (STM32L072, $(strip $(MCU)))
	SRCS += $(STM32_PORTING_DIR)/stm32l072xx.c
endif