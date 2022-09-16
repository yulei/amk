PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/gd32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/gd32.c \
	$(MAIN_DIR)/porting/wired/common/cm_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifeq (GD32E505, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/gd32e505.c
endif
