LPC55SDK_DIR := sdk/lpc55

ifeq (LPC5516, $(strip $(MCU)))
MCU_ARCH := CORTEX_M33
MCU_DEVICE:= LPC5516
MCU_TYPE := lpc5516
endif

SRCS += \
	$(LPC55SDK_DIR)/drivers/fsl_clock.c \
	$(LPC55SDK_DIR)/drivers/fsl_common_arm.c \
	$(LPC55SDK_DIR)/drivers/fsl_common.c \
	$(LPC55SDK_DIR)/drivers/fsl_flexcomm.c \
	$(LPC55SDK_DIR)/drivers/fsl_gpio.c \
	$(LPC55SDK_DIR)/drivers/fsl_i2c.c \
	$(LPC55SDK_DIR)/drivers/fsl_lpadc.c \
	$(LPC55SDK_DIR)/drivers/fsl_power.c \
	$(LPC55SDK_DIR)/drivers/fsl_reset.c \
	$(LPC55SDK_DIR)/drivers/fsl_spi.c \
	$(LPC55SDK_DIR)/drivers/fsl_usart.c \

INCS += \
	$(LPC55SDK_DIR)/drivers \

SRCS += \
	$(LPC55SDK_DIR)/device/$(MCU_DEVICE)/gcc/startup_$(MCU_DEVICE).S \
	$(LPC55SDK_DIR)/device/$(MCU_DEVICE)/system_$(MCU_DEVICE).c \

INCS += $(LPC55SDK_DIR)/device/$(MCU_DEVICE) \

include $(LIB_DIR)/cmsis_core.mk
#include $(LIB_DIR)/cmsis_compiler.mk

APP_DEFS += -DLPC55xx

include $(LPC55SDK_DIR)/$(MCU_TYPE).mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
	APP_DEFS += -DCFG_TUSB_MCU=OPT_MCU_LPC55XX
	APP_DEFS += -DCFG_TUSB_MEM_ALIGN='__attribute__((aligned(64)))'
	APP_DEFS += -DBOARD_DEVICE_RHPORT_NUM=1
  	APP_DEFS += -DBOARD_TUD_RHPORT=1
	APP_DEFS += -DBOARD_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED
	APP_DEFS += -DCFG_TUSB_MEM_SECTION='__attribute__((section("m_usb_global")))'
endif #TINYUSB_ENABLE

LINKER_PATH := $(LPC55SDK_DIR)/device/$(MCU_DEVICE)/gcc
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
