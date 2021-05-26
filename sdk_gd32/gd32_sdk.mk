
GD32SDK_DIR := $(VENDOR_DIR)/gd32e10x
GD32SDK_PERI_DIR := $(VENDOR_DIR)/gd32e10x/GD32E10x_standard_peripheral
GD32SDK_USB_DIR := $(VENDOR_DIR)/gd32e10x/GD32E10x_usbfs_library

ifeq (GD32E103, $(strip $(MCU)))
MCU_SERIES := f4
MCU_FAMILY := gd32e10x 
MCU_TYPE := e103
endif


SRCS += \
	$(GD32SDK_DIR)/gcc/startup_gd32e103.s \
	$(GD32SDK_DIR)/CMSIS/GD/GD32E10x/Source/system_gd32e10x.c \
	$(GD32SDK_PERI_DIR)/Source/gd32e10x_fmc.c \
	$(GD32SDK_PERI_DIR)/Source/gd32e10x_gpio.c \
	$(GD32SDK_PERI_DIR)/Source/gd32e10x_misc.c \
	$(GD32SDK_PERI_DIR)/Source/gd32e10x_pmu.c \
	$(GD32SDK_PERI_DIR)/Source/gd32e10x_rcu.c \
	$(GD32SDK_PERI_DIR)/Source/gd32e10x_timer.c \
	$(GD32SDK_USB_DIR)/driver/Source/drv_usb_core.c \
	$(GD32SDK_USB_DIR)/driver/Source/drv_usb_dev.c \
	$(GD32SDK_USB_DIR)/driver/Source/drv_usbd_int.c \
	$(GD32SDK_USB_DIR)/device/core/Source/usbd_core.c \
	$(GD32SDK_USB_DIR)/device/core/Source/usbd_enum.c \
	$(GD32SDK_USB_DIR)/device/core/Source/usbd_transc.c \

INCS += \
	$(GD32SDK_DIR)/CMSIS \
	$(GD32SDK_DIR)/CMSIS/GD/GD32E10x/Include \
	$(GD32SDK_PERI_DIR)/Include \
	$(GD32SDK_USB_DIR)/driver/Include \
	$(GD32SDK_USB_DIR)/device/core/Include \
	$(GD32SDK_USB_DIR)/device/class/hid/Include \
	$(GD32SDK_USB_DIR)/ustd/class/hid \
	$(GD32SDK_USB_DIR)/ustd/common \

APP_DEFS += \
	-DCFG_TUSB_MCU=GD32E10X \
	-DUSE_STDPERIPH_DRIVER \
	-DGD32E10X \
	-DHXTAL_VALUE=25000000 \
	-DHXTAL_VALUE_25M=HXTAL_VALUE \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

LINKER_PATH	?= $(GD32SDK_DIR)/gcc
LINKER_SCRIPT := $(LINKER_PATH)/gd32e103xb_flash.ld