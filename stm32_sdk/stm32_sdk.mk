STM32SDK_DIR := stm32_sdk

ifeq (STM32F411, $(strip $(MCU)))
include $(STM32SDK_DIR)/f411_sdk.mk
include lib/tinyusb.mk
endif

ifeq (STM32F405, $(strip $(MCU)))
include $(STM32SDK_DIR)/f405_sdk.mk
include $(STM32SDK_DIR)/f4_usb.mk
endif

ifeq (STM32F722, $(strip $(MCU)))
include $(STM32SDK_DIR)/f7_sdk.mk
endif

ifeq (STM32F103, $(strip $(MCU)))
include $(STM32SDK_DIR)/f1_sdk.mk
include lib/tinyusb.mk
endif