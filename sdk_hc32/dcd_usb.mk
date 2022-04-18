HC32USB_DIR := $(VENDOR_DIR)/$(HC32_DDL)/usb_lib

SRCS += \
	$(HC32USB_DIR)/usb_core/usb_core_driver.c \
	$(HC32SDK_DIR)/ddl_dcd_hc32.c \
	$(HC32SDK_DIR)/ddl_dcd_int.c \
	$(HC32SDK_DIR)/ddl_bsp_usb.c \

INCS += \
	$(HC32USB_DIR)/usb_core \
	$(HC32SDK_DIR) \

APP_DEFS += \
	-DTUP_DCD_ENDPOINT_MAX=6 \
