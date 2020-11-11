STM32SDK_DIR := $(STM32_DIR)
USB_WITH_TINYUSB ?= no

ifeq (STM32F411, $(strip $(MCU)))
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f411
USB_WITH_TINYUSB := yes
endif

ifeq (STM32F405, $(strip $(MCU)))
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f405
endif

ifeq (STM32F722, $(strip $(MCU)))
MCU_SERIES := f7
MCU_FAMILY := stm32f7xx
MCU_TYPE := f722
endif

ifeq (STM32F103, $(strip $(MCU)))
MCU_SERIES := f1
MCU_FAMILY := stm32f1xx
MCU_TYPE := f103
USB_WITH_TINYUSB := yes
endif

SRCS += \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/system_$(MCU_FAMILY).c \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_hal_msp.c \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_it.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_cortex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_exti.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_uart.c

INCS += \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE) \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Inc \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Inc/Legacy \
	$(STM32SDK_DIR)/devices/$(MCU_SERIES)/Include \
	$(STM32SDK_DIR)/cmsis/Core/Include

APP_DEFS += -DUSE_HAL_DRIVER

include $(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(USB_WITH_TINYUSB)))
include $(LIB_DIR)/tinyusb.mk
else
include $(STM32SDK_DIR)/hal_usb.mk
endif