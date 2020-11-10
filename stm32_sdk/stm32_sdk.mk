STM32SDK_DIR := $(STM32_DIR)

ifeq (STM32F411, $(strip $(MCU)))
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_PATH := f411
include $(STM32SDK_DIR)/f411_sdk.mk
include $(LIB_DIR)/tinyusb.mk
endif

ifeq (STM32F405, $(strip $(MCU)))
MCU_FAMILY := f4
include $(STM32SDK_DIR)/f405_sdk.mk
include $(STM32SDK_DIR)/f4_usb.mk
endif

ifeq (STM32F722, $(strip $(MCU)))
MCU_FAMILY := f7
include $(STM32SDK_DIR)/f7_sdk.mk
include $(LIB_DIR)/tinyusb.mk
endif

ifeq (STM32F103, $(strip $(MCU)))
MCU_SERIES := f1
MCU_FAMILY := stm32f1xx
include $(STM32SDK_DIR)/f1_sdk.mk
include $(LIB_DIR)/tinyusb.mk
endif

INCS += \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Inc \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Inc/Legacy \
	$(STM32SDK_DIR)/devices/$(MCU_SERIES)/Include \
	$(STM32SDK_DIR)/cmsis5/CMSIS/Core/Include \

APP_DEFS += -DUSE_HAL_DRIVER