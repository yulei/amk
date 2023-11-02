STM32SDK_DIR := sdk_stm32

ifeq (STM32F401, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f401
endif

ifeq (STM32F411, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f411
endif

ifeq (STM32F405, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
ifeq (yes, $(strip $(USE_F405_V2)))
MCU_TYPE := f405_v2
else
	ifeq (yes, $(strip $(USE_F405_APM)))
	MCU_TYPE := f405_apm
	else
	MCU_TYPE := f405
	endif
endif
#TINYUSB_ENABLE := no
endif

ifeq (STM32F446, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f446
endif

ifeq (STM32F722, $(strip $(MCU)))
MCU_ARCH := CORTEX_M7
MCU_SERIES := f7
MCU_FAMILY := stm32f7xx
MCU_TYPE := f722
endif

ifeq (STM32F103, $(strip $(MCU)))
MCU_ARCH := CORTEX_M3
MCU_SERIES := f1
MCU_FAMILY := stm32f1xx
MCU_TYPE := f103
endif

ifeq (STM32L432, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := l4
MCU_FAMILY := stm32l4xx
MCU_TYPE := l432
endif

ifeq (STM32L072, $(strip $(MCU)))
MCU_ARCH := CORTEX_M0
MCU_SERIES := l0
MCU_FAMILY := stm32l0xx
MCU_TYPE := l072
endif

ifeq (STM32G431, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := g4
MCU_FAMILY := stm32g4xx
MCU_TYPE := g431
endif

ifeq (STM32L452, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := l4
MCU_FAMILY := stm32l4xx
MCU_TYPE := l452
endif

ifeq (STM32F412, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := f4
MCU_FAMILY := stm32f4xx
MCU_TYPE := f412
endif

ifeq (STM32L476, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := l4
MCU_FAMILY := stm32l4xx
MCU_TYPE := l476
endif

SRCS += \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim.c \
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
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Inc \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Inc/Legacy \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Include \
	$(LIB_DIR)/cmsis_5/CMSIS/Core/Include

ifeq (yes, $(strip $(AUDIO_ENABLE)))
include $(LIB_DIR)/cmsis_dsp.mk
endif

APP_DEFS += -DUSE_HAL_DRIVER

ifeq (,$(filter $(strip $(MCU)), STM32L432 STM32L452 STM32F412))
include $(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_TYPE)_sdk.mk
SRCS +=	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/system_$(MCU_FAMILY).c
SRCS +=	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_hal_msp.c
SRCS +=	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_it.c
INCS +=	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)
else
include $(STM32SDK_DIR)/mcus/$(MCU_SERIES)/$(MCU_TYPE)_sdk.mk
SRCS +=	$(STM32SDK_DIR)/mcus/$(MCU_SERIES)/system_$(MCU_FAMILY).c
SRCS +=	$(STM32SDK_DIR)/mcus/$(MCU_SERIES)/$(MCU_FAMILY)_hal_msp.c
SRCS +=	$(STM32SDK_DIR)/mcus/$(MCU_SERIES)/$(MCU_FAMILY)_it.c
INCS +=	$(STM32SDK_DIR)/mcus/$(MCU_SERIES)
endif

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
	ifneq (yes, $(strip $(RTOS_ENABLE)))
		include $(STM32SDK_DIR)/hal_usb.mk
	endif
	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd.c 
	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd_ex.c
	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c 
	APP_DEFS += -DHAL_PCD_MODULE_ENABLED
	APP_DEFS += -DHALUSB_ENABLE
endif #TINYUSB_ENABLE

LINKER_PATH	?= $(STM32SDK_DIR)/mcus/linker
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_FLASH.ld
