PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/at32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/at32.c \
	$(MAIN_DIR)/porting/wired/common/cm_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifeq (AT32F405, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/at32f405.c
endif
