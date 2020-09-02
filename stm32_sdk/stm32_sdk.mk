STM32SDK_DIR := $(TOP_DIR)/stm32_sdk

ifeq (STM32F411, $(strip $(MCU)))
include $(STM32SDK_DIR)/f4_sdk.mk
endif

ifeq (STM32L433, $(strip $(MCU)))
include $(STM32SDK_DIR)/l4_sdk.mk
endif