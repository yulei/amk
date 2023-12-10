ATSAMDSDK_DIR := sdk/samd

ifeq (ATSAMD21, $(strip $(MCU)))
MCU_SERIES := atsam
MCU_FAMILY := d21
MCU_TYPE := samd21a
endif

SRCS += \
	$(VENDOR_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/gcc/gcc/startup_samd21.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/gcc/system_samd21.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_atomic.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_delay.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_flash.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_gpio.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_init.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_io.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_sleep.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/src/hal_usart_sync.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_assert.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_event.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_list.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_syscalls.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/core/hpl_init.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/core/hpl_core_m0plus_base.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/dmac/hpl_dmac.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/gclk/hpl_gclk.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/nvmctrl/hpl_nvmctrl.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/pm/hpl_pm.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/sercom/hpl_sercom.c \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/sysctrl/hpl_sysctrl.c \

INCS += \
	$(VENDOR_DIR)/$(MCU_FAMILY) \
	$(VENDOR_DIR)/$(MCU_FAMILY)/CMSIS/Core/Include \
	$(VENDOR_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/include \
	$(VENDOR_DIR)/$(MCU_FAMILY)/config \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/core \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/dmac \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/gclk \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/include \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hal/utils/include \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/pm \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/port \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/sercom \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hpl/sysctrl \
	$(VENDOR_DIR)/$(MCU_FAMILY)/hri \

APP_DEFS += \

include $(VENDOR_DIR)/$(MCU_SERIES)$(MCU_FAMILY).mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
endif

LINKER_PATH	?= $(VENDOR_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/gcc/gcc
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
