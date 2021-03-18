TINYUSB_ENABLE := yes
ATSAMD_PORTING_DIR := $(MAIN_DIR)/porting/atsamd

SRCS += \
	$(ATSAMD_PORTING_DIR)/timer.c \
	$(ATSAMD_PORTING_DIR)/bootloader.c \
	$(ATSAMD_PORTING_DIR)/wait.c \
	$(ATSAMD_PORTING_DIR)/generic_atsamd.c \
	$(ATSAMD_PORTING_DIR)/gpio_pin.c \
	$(ATSAMD_PORTING_DIR)/usb_tinyusb.c \

INCS += \
	$(ATSAMD_PORTING_DIR) \

ifeq (ATSAMD21, $(strip $(MCU)))
	SRCS += $(ATSAMD_PORTING_DIR)/atsamd21.c
endif
