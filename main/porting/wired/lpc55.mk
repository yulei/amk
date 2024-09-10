PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/lpc55

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/lpc55.c \
	$(MAIN_DIR)/porting/wired/common/cm_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifeq (LPC5516, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/lpc5516.c
endif
