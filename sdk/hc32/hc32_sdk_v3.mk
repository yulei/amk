HC32SDK_DIR := sdk_hc32

ifeq (HC32F460, $(strip $(MCU)))
MCU_SERIES := hc32 
MCU_TYPE := f460
HC32_DDL := HC32F460_DDL_Rev3.0.0
endif

SRCS += \
	$(VENDOR_DIR)/$(HC32_DDL)/drivers/Device/HDSC/hc32f4xx/Souce/GCC/startup_hc32f460.S \
	$(VENDOR_DIR)/$(HC32_DDL)/drivers/Device/HDSC/hc32f4xx/Souce/system_hc32f460.c \

SRCS += $(wildcard $(VENDOR_DIR)/$(HC32_DDL)/drivers/hc32_ll_driver/*.c)

INCS += \
	$(VENDOR_DIR)/$(HC32_DDL)/drivers/cmsis/Device/HDSC/hc32f4xx/Include \
	$(VENDOR_DIR)/$(HC32_DDL)/drivers/cmsis/Include \
	$(VENDOR_DIR)/$(HC32_DDL)/drivers/hc32_ll_driver/inc \
	$(HC32SDK_DIR) \

APP_DEFS += \
	-DUSE_DEVICE_DRIVER_LIB \

include $(HC32SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
#include $(HC32SDK_DIR)/dcd_usb_ddl.mk
	include $(HC32SDK_DIR)/dcd_usb_dwc2.mk
	APP_DEFS += -DTINYUSB_ENABLE
endif

LINKER_PATH	?= $(VENDOR_DIR)/$(HC32_DDL)/$(VENDOR_DIR)/$(HC32_DDL)/drivers/Device/HDSC/hc32f4xx/Souce/GCC/linker
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD).ld
