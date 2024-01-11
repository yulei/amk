GD32SDK_DIR := sdk/gd32

ifeq (GD32E103, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_PREFIX := GD32E10x
MCU_SERIES := gd32e10x
MCU_TYPE := e103
GD32_LIB_VER := 1.2.2
GD32_LIB_DIR := $(MCU_PREFIX)_Firmware_Library_V$(GD32_LIB_VER)
GD32_LIB_STD := $(MCU_PREFIX)_standard_peripheral
endif

ifeq (GD32E505, $(strip $(MCU)))
MCU_ARCH := CORTEX_M33
MCU_PREFIX := GD32E50x
MCU_SERIES := gd32e50x
MCU_TYPE := e505
GD32_LIB_VER := 1.3.0
GD32_LIB_DIR := $(MCU_PREFIX)_Firmware_Library_V$(GD32_LIB_VER)
GD32_LIB_STD := $(MCU_PREFIX)_standard_peripheral
endif

SRCS += \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_adc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_bkp.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_dac.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_dbg.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_dma.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_exmc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_exti.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_fmc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_fwdgt.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_gpio.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_i2c.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_misc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_pmu.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_rcu.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_rtc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_spi.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_timer.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_usart.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Source/$(MCU_SERIES)_wwdgt.c \

INCS += \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/CMSIS/GD/$(MCU_SERIES)/Include \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/$(GD32_LIB_STD)/Include \
	${GD32SDK_DIR} \
	${GD32SDK_DIR}/$(MCU_TYPE) \

include $(LIB_DIR)/cmsis_core.mk

APP_DEFS += \

include $(GD32SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	include $(GD32SDK_DIR)/dcd_usb_dwc2.mk
	APP_DEFS += -DTINYUSB_ENABLE
endif

LINKER_PATH	?= $(GD32SDK_DIR)/$(MCU_TYPE)
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
