ATSAMDSDK_DIR := $(ATSAMD_DIR)

ifeq (ATSAMD21, $(strip $(MCU)))
MCU_SERIES := atsam
MCU_FAMILY := d21
MCU_TYPE := samd21a
endif

SRCS += \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/gcc/gcc/startup_samd21.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/gcc/system_samd21.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_atomic.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_delay.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_flash.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_gpio.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_init.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_io.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_sleep.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/src/hal_usart_sync.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_assert.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_event.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_list.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/utils/src/utils_syscalls.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/core/hpl_init.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/core/hpl_core_m0plus_base.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/dmac/hpl_dmac.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/gclk/hpl_gclk.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/nvmctrl/hpl_nvmctrl.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/pm/hpl_pm.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/sercom/hpl_sercom.c \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/sysctrl/hpl_sysctrl.c \

INCS += \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY) \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/CMSIS/Core/Include \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/include \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/config \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/core \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/dmac \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/gclk \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/include \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hal/utils/include \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/pm \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/port \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/sercom \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hpl/sysctrl \
	$(ATSAMDSDK_DIR)/$(MCU_FAMILY)/hri \

APP_DEFS += \

include $(ATSAMDSDK_DIR)/$(MCU_SERIES)$(MCU_FAMILY).mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
endif

LINKER_PATH	?= $(ATSAMDSDK_DIR)/$(MCU_FAMILY)/$(MCU_TYPE)/gcc/gcc
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
