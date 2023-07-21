APM32SDK_DIR := sdk_apm32

ifeq (APM32F407, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4 
MCU_PREFIX := geehy
MCU_SERIES := apm32f4xx
MCU_TYPE := f407
APM32_LIB_DIR := APM32F4xx
APM32_LIB_STD := $(APM32_LIB_DIR)_StdPeriphDriver
endif

SRCS += \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_adc.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_dbgmcu.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_dma.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_fmc.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_gpio.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_i2c.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_misc.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_pmu.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_rcm.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_rtc.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_spi.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_syscfg.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_tmr.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_usart.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_usb_device.c \
	${VENDOR_DIR}/${APM32_LIB_STD}/src/${MCU_SERIES}_usb.c \

INCS += \
	${LIB_DIR}/cmsis_5/CMSIS/Core/Include \
	${VENDOR_DIR}/${APM32_LIB_DIR}/include \
	${VENDOR_DIR}/${APM32_LIB_STD}/inc \
	${APM32SDK_DIR} \
	${APM32SDK_DIR}/$(MCU_TYPE) \

APP_DEFS += \

include $(APM32SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	SRCS += $(APM32SDK_DIR)/$(MCU_TYPE)/dcd_hal_apm32.c
	APP_DEFS += -DTINYUSB_ENABLE
endif

LINKER_PATH	?= $(APM32SDK_DIR)/$(MCU_TYPE)
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_FLASH.ld
