COMMON_PORTING_DIR := $(MAIN_DIR)/porting/common

SRCS += \
	$(MAIN_DIR)/rtt/SEGGER_RTT.c

SRCS += \
	$(COMMON_PORTING_DIR)/bootloader.c \
	$(COMMON_PORTING_DIR)/generic_board.c \
	$(COMMON_PORTING_DIR)/suspend.c \
	$(COMMON_PORTING_DIR)/timer.c \

INCS += \
	$(COMMON_PORTING_DIR) \

ifeq (yes, $(TINYUSB_ENABLE))
	SRCS += $(COMMON_PORTING_DIR)/usb_tinyusb.c
endif