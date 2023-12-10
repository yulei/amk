M480SDK_DIR := sdk/m480

ifeq (M484, $(strip $(MCU)))
MCU_ARCH := CORTEX_M4
MCU_SERIES := m480 
MCU_TYPE := m484
M480_STD := m480/Library
endif

SRCS += \
	$(VENDOR_DIR)/$(M480_STD)/Device/Nuvoton/M480/Source/GCC/startup_M480.S \
	$(VENDOR_DIR)/$(M480_STD)/Device/Nuvoton/M480/Source/system_M480.c \
	$(VENDOR_DIR)/$(M480_STD)/StdDriver/src/clk.c \
	$(VENDOR_DIR)/$(M480_STD)/StdDriver/src/gpio.c \
	$(VENDOR_DIR)/$(M480_STD)/StdDriver/src/sys.c \
	$(M480SDK_DIR)/m480_fmc.c \
#	$(VENDOR_DIR)/$(M480_STD)/StdDriver/src/fmc.c \

INCS += \
	$(VENDOR_DIR)/$(M480_STD)/Device/Nuvoton/M480/Include \
	$(VENDOR_DIR)/$(M480_STD)/StdDriver/inc \
	$(VENDOR_DIR)/$(M480_STD)/StdDriver/src \
	$(M480SDK_DIR) \
	$(LIB_DIR)/cmsis_5/CMSIS/Core/Include



APP_DEFS += \

include $(M480SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
	SRCS += $(M480SDK_DIR)/dcd_m480.c
endif

LINKER_PATH	?= $(M480SDK_DIR)/
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
