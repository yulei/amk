PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/gd32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/wait.c \
	$(PLATFORM_PORTING_DIR)/gd32.c \
	$(PLATFORM_PORTING_DIR)/gd32_util.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifeq (GD32E505, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/gd32e505.c
endif

ifeq (YES, $(strip $(TINYUSB_ENABLE)))
SRCS += $(PLATFORM_PORTING_DIR)/gd32_tinyusb.c
endif
