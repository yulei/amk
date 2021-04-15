include $(MAIN_DIR)/porting/common_porting.mk

STM32_PORTING_DIR := $(MAIN_DIR)/porting/stm32

SRCS += \
	$(STM32_PORTING_DIR)/eeconfig_flash.c \
	$(STM32_PORTING_DIR)/gpio_pin.c \
	$(STM32_PORTING_DIR)/stm32.c \
	$(STM32_PORTING_DIR)/wait.c \

INCS += \
	$(STM32_PORTING_DIR) \

ifneq (yes, $(TINYUSB_ENABLE))
	SRCS += $(STM32_PORTING_DIR)/usb_hal.c
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