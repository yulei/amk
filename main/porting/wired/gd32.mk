
GD32_PORTING_DIR := $(MAIN_DIR)/porting/wired/gd32

SRCS += \
	$(GD32_PORTING_DIR)/gd32e10x_hw.c \
	$(GD32_PORTING_DIR)/gpio_pin.c \
	$(GD32_PORTING_DIR)/gd32.c \
	$(GD32_PORTING_DIR)/wait.c \
	$(GD32_PORTING_DIR)/usb_gd32.c \
	$(GD32_PORTING_DIR)/usb_comp.c \

INCS += \
	$(GD32_PORTING_DIR) \
