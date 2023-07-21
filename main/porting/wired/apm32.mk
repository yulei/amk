PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/apm32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/apm32.c \
	$(MAIN_DIR)/porting/wired/common/cm_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifeq (APM32F407, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/apm32f407.c
endif
