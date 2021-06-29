STM32SDK_DIR := sdk_stm32

ifeq (STM32F411, $(strip $(MCU)))
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f411
endif

ifeq (STM32F405, $(strip $(MCU)))
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f405
TINYUSB_ENABLE := no
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
endif

ifeq (STM32L432, $(strip $(MCU)))
MCU_SERIES := l4
MCU_FAMILY := stm32l4xx
MCU_TYPE := l432
endif

ifeq (STM32L072, $(strip $(MCU)))
MCU_SERIES := l0
MCU_FAMILY := stm32l0xx
MCU_TYPE := l072
endif

SRCS += \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/system_$(MCU_FAMILY).c \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_hal_msp.c \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_it.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rtc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rtc_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_cortex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_exti.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_uart.c

INCS += \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE) \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Inc \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Inc/Legacy \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Include \
	$(VENDOR_DIR)/cmsis/Core/Include \
	$(VENDOR_DIR)/cmsis/DSP/Include

APP_DEFS += -DUSE_HAL_DRIVER

include $(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
	ifeq (yes, $(strip $(TINYUSB_USE_HAL)))
		SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd.c 
		SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd_ex.c
		SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c 
		APP_DEFS += -DHAL_PCD_MODULE_ENABLED
	endif #TINYUSB_USE_HAL
else
	include $(STM32SDK_DIR)/hal_usb.mk
	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd.c 
	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd_ex.c
	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c 
	APP_DEFS += -DHAL_PCD_MODULE_ENABLED
endif #TINYUSB_ENABLE

LINKER_PATH	?= $(STM32SDK_DIR)/mcus/linker
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_FLASH.ld
