AT32SDK_DIR := sdk/at32

ifeq (AT32F405, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4 
MCU_PREFIX := artery 
MCU_SERIES := at32f402_405
MCU_TYPE := f405
AT32_LIB_DIR := AT32F402_405_Firmware_Library_V2.0.9
endif

SRCS += \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_adc.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_crm.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_debug.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_dma.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_ertc.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_flash.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_gpio.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_i2c.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_misc.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_pwc.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_qspi.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_scfg.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_spi.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_usart.c \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/src/${MCU_SERIES}_wdt.c \

INCS += \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/cmsis/cm4/device_support \
	${VENDOR_DIR}/${AT32_LIB_DIR}/libraries/drivers/inc \
	${VENDOR_DIR}/${AT32_LIB_DIR}/middlewares/i2c_application_library \
	${AT32SDK_DIR} \
	${AT32SDK_DIR}/$(MCU_TYPE) \

APP_DEFS += \
	-DUSE_STDPERIPH_DRIVER \

include $(AT32SDK_DIR)/$(MCU_TYPE)_sdk.mk
include $(LIB_DIR)/cmsis_core.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	SRCS += $(AT32SDK_DIR)/$(MCU_TYPE)/dcd_dwc2.c
	APP_DEFS += -DTINYUSB_ENABLE
	APP_DEFS += -DUSB_OTG_HS
endif

LINKER_PATH	?= $(AT32SDK_DIR)/$(MCU_TYPE)
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_FLASH.ld

