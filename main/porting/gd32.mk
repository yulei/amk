include $(MAIN_DIR)/porting/common_porting.mk

GD32_PORTING_DIR := $(MAIN_DIR)/porting/gd32

SRCS += \
	$(GD32_PORTING_DIR)/gd32e10x_hw.c \
	$(GD32_PORTING_DIR)/gpio_pin.c \
	$(GD32_PORTING_DIR)/gd32.c \
	$(GD32_PORTING_DIR)/wait.c \
	$(GD32_PORTING_DIR)/usb_gd32.c \
	$(GD32_PORTING_DIR)/usb_comp.c \

INCS += \
	$(GD32_PORTING_DIR) \

ifeq (yes, $(EECONFIG_FRAM)) 
	SRCS += $(MAIN_DIR)/eeconfig_fram.c
	SRCS += $(MAIN_DIR)/drivers/mb85rcxx.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DEECONFIG_FRAM
else
	ifeq (yes, $(EECONFIG_FLASH))
		SRCS += $(MAIN_DIR)/eeconfig_flash.c
		APP_DEFS += -DEECONFIG_FLASH
	else
		SRCS += $(MAIN_DIR)/eeconfig_mem.c
	endif
endif
