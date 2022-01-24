
#USB_DEVICE_PATH := $(VENDOR_DIR)/usb_device
#USB_HOST_PATH := $(VENDOR_DIR)/usb_host
USB_DEVICE_PATH := $(STM32SDK_DIR)/hal_usb/usbd_core
USB_HOST_PATH := $(STM32SDK_DIR)/hal_usb/usbh_core
HAL_USB_PATH :=$(STM32SDK_DIR)/hal_usb

SRCS += \
	$(HAL_USB_PATH)/usb_device.c \
	$(HAL_USB_PATH)/usbd_conf.c \
	$(HAL_USB_PATH)/usbd_desc.c \
	$(HAL_USB_PATH)/usbd_composite.c \
	$(USB_DEVICE_PATH)/usbd_core.c \
	$(USB_DEVICE_PATH)/usbd_ctlreq.c \
	$(USB_DEVICE_PATH)/usbd_ioreq.c

ifeq (yes, $(strip $(USB_HOST_ENABLE)))
SRCS += \
	$(HAL_USB_PATH)/report_parser.c \
	$(HAL_USB_PATH)/usbh_hid_multi.c \
	$(HAL_USB_PATH)/usb_host.c \
	$(HAL_USB_PATH)/usbh_conf.c \
	$(USB_HOST_PATH)/usbh_core.c \
	$(USB_HOST_PATH)/usbh_ctlreq.c \
	$(USB_HOST_PATH)/usbh_ioreq.c \
	$(USB_HOST_PATH)/usbh_pipes.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c

APP_DEFS += -DUSB_HOST_ENABLE -DHAL_HCD_MODULE_ENABLED
endif

INCS += \
	$(HAL_USB_PATH) \
	$(USB_DEVICE_PATH) \
	$(USB_HOST_PATH) \
	$(LIB_DIR)/tinyusb/src 
