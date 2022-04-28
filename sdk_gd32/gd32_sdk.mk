GD32SDK_DIR := sdk_gd32

ifeq (GD32E103, $(strip $(MCU)))
MCU_SERIES := gd32
MCU_TYPE := e103
GD32_LIB_DIR:= GD32E10x_Firmware_Library_V1.2.2
endif

SRCS += \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_adc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_bkp.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_crc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_ctc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_dac.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_dbg.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_dma.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_exmc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_exti.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_fmc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_fwdgt.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_gpio.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_i2c.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_misc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_pmu.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_rcu.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_rtc.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_spi.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_timer.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_usart.c \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Source/gd32e10x_wwdgt.c \

INCS += \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/CMSIS \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/CMSIS/GD/GD32E10x/Include \
	${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_standard_peripheral/Include \
	${GD32SDK_DIR}

APP_DEFS += \

include $(GD32SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(USB_HOST_ENABLE)))
#	include $(GD32SDK_DIR)/gd32_usbhost.mk
	include $(GD32SDK_DIR)/gd32_tinyusb.mk
	APP_DEFS += -DUSB_HOST_ENABLE
endif

LINKER_PATH	?= $(GD32SDK_DIR)
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
