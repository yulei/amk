include $(MAIN_DIR)/porting/common_porting.mk

PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/stm32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/stm32.c \
	$(PLATFORM_PORTING_DIR)/wait.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifneq (yes, $(TINYUSB_ENABLE))
	SRCS += $(PLATFORM_PORTING_DIR)/usb_hal.c
endif

ifeq (STM32F411, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f411xe.c
endif

ifeq (STM32F405, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f405xx.c
endif

ifeq (STM32F722, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f722xx.c
endif

ifeq (STM32F103, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f103xb.c
endif

ifeq (STM32L432, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32l432xx.c
endif

ifeq (STM32L072, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32l072xx.c
endif