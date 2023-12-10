CH32SDK_DIR := sdk/ch32

ifeq (CH32V307, $(strip $(MCU)))
MCU_ARCH := RV32
MCU_PREFIX := ch32v30x
MCU_SERIES := ch32v30x
MCU_TYPE := v307
CH32_LIB_DIR := $(MCU_PREFIX)
CH32_LIB_STD := $(CH32_LIB_DIR)/Peripheral
endif

SRCS += \
	${VENDOR_DIR}/${CH32_LIB_DIR}/Core/core_riscv.c \
	${VENDOR_DIR}/${CH32_LIB_DIR}/Debug/debug.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_bkp.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_flash.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_gpio.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_i2c.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_misc.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_pwr.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_rcc.c \
	${VENDOR_DIR}/${CH32_LIB_STD}/src/$(MCU_SERIES)_usart.c \

INCS += \
	${VENDOR_DIR}/${CH32_LIB_DIR}/Core \
	${VENDOR_DIR}/${CH32_LIB_STD}/inc \
	${VENDOR_DIR}/${CH32_LIB_DIR} \
	${CH32SDK_DIR} \
	${CH32SDK_DIR}/$(MCU_TYPE) \

APP_DEFS += \
	-DNO_PRINT

include $(CH32SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	SRCS += $(CH32SDK_DIR)/$(MCU_TYPE)/dcd_usbhs.c
	APP_DEFS += -DTINYUSB_ENABLE
endif

LINKER_PATH	?= $(CH32SDK_DIR)/$(MCU_TYPE)
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
