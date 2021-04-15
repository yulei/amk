include $(MAIN_DIR)/porting/common_porting.mk

NUVOTON_PORTING_DIR := $(MAIN_DIR)/porting/nuvoton

SRCS += \
	$(NUVOTON_PORTING_DIR)/gpio_pin.c \
	$(NUVOTON_PORTING_DIR)/nuvoton.c \
	$(NUVOTON_PORTING_DIR)/wait.c \
	$(MAIN_DIR)/eeconfig_mem.c

INCS += \
	$(NUVOTON_PORTING_DIR) \

ifeq (yes, $(TINYUSB_ENABLE))
	SRCS += $(COMMON_PORTING_DIR)/usb_tinyusb.c
endif