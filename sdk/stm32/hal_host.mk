
USB_HOST_PATH := $(STM32SDK_DIR)/hal_usb/usbh_core
HAL_USB_PATH :=$(STM32SDK_DIR)/hal_usb

SRCS += \
	$(HAL_USB_PATH)/hal_host.c \
	$(HAL_USB_PATH)/usb_host.c \
	$(HAL_USB_PATH)/report_parser.c \
	$(HAL_USB_PATH)/usbh_hid_multi.c \
	$(USB_HOST_PATH)/usbh_core.c \
	$(USB_HOST_PATH)/usbh_ctlreq.c \
	$(USB_HOST_PATH)/usbh_ioreq.c \
	$(USB_HOST_PATH)/usbh_pipes.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c

APP_DEFS += -DUSB_HOST_ENABLE -DHAL_HCD_MODULE_ENABLED -DHAL_HOST_ENABLE

INCS += \
	$(HAL_USB_PATH) \
	$(USB_HOST_PATH) \
