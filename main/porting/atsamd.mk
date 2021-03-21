include $(MAIN_DIR)/porting/common_porting.mk

TINYUSB_ENABLE := yes
ATSAMD_PORTING_DIR := $(MAIN_DIR)/porting/atsamd

SRCS += \
	$(ATSAMD_PORTING_DIR)/atsamd.c \
	$(ATSAMD_PORTING_DIR)/gpio_pin.c \
	$(ATSAMD_PORTING_DIR)/wait.c \
	$(MAIN_DIR)/eeconfig_mem.c

INCS += \
	$(ATSAMD_PORTING_DIR) \

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	SRCS += $(COMMON_PORTING_DIR)/usb_tinyusb.c
endif