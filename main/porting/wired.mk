COMMON_PORTING_DIR := $(MAIN_DIR)/porting/wired/common

SRCS += \
	$(MAIN_DIR)/segger/SEGGER_RTT.c

SRCS += \
	$(COMMON_PORTING_DIR)/bootloader.c \
	$(COMMON_PORTING_DIR)/amk_driver.c \
	$(COMMON_PORTING_DIR)/suspend.c \
	$(COMMON_PORTING_DIR)/timer.c \
	$(COMMON_PORTING_DIR)/usb_interface.c \

INCS += \
	$(COMMON_PORTING_DIR) \

ifneq (yes, $(strip $(RTOS_ENABLE)))
	SRCS += $(COMMON_PORTING_DIR)/wired_board.c
endif

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	SRCS += $(COMMON_PORTING_DIR)/usb_tinyusb.c
endif

ifeq (yes, $(strip $(RF_ENABLE)))
	SRCS += $(COMMON_PORTING_DIR)/rf_driver.c
	APP_DEFS += -DRF_ENABLE
endif

ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
	include $(MAIN_DIR)/porting/wired/stm32.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(ATSAMD_MCUS)))
	include $(MAIN_DIR)/porting/wired/atsamd.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(HC32_MCUS)))
	include $(MAIN_DIR)/porting/wired/hc32.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(GD32_MCUS)))
	include $(MAIN_DIR)/porting/wired/gd32.mk
endif