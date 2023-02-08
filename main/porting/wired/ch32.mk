PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/ch32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/ch32.c \
	$(MAIN_DIR)/porting/wired/common/rv32_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifeq (CH32V307, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/ch32v307.c
endif
