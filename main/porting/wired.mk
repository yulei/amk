COMMON_PORTING_DIR := $(MAIN_DIR)/porting/wired/common

SRCS += \
	$(MAIN_DIR)/rtt/SEGGER_RTT.c

SRCS += \
	$(COMMON_PORTING_DIR)/bootloader.c \
	$(COMMON_PORTING_DIR)/generic_board.c \
	$(COMMON_PORTING_DIR)/suspend.c \
	$(COMMON_PORTING_DIR)/timer.c \



INCS += \
	$(COMMON_PORTING_DIR) \

ifneq (yes, $(RTOS_ENABLE))
	SRCS += $(COMMON_PORTING_DIR)/usb_interface.c
endif

ifeq (yes, $(TINYUSB_ENABLE))
	SRCS += $(COMMON_PORTING_DIR)/usb_tinyusb.c
endif

ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
	include $(MAIN_DIR)/porting/wired/stm32.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(ATSAMD_MCUS)))
	include $(MAIN_DIR)/porting/wired/atsamd.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(NUVOTON_MCUS)))
	include $(MAIN_DIR)/porting/wired/nuvoton.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(GD32_MCUS)))
	include $(MAIN_DIR)/porting/wired/gd32.mk
endif