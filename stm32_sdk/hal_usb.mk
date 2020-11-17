
USB_DEVICE_PATH := $(STM32SDK_DIR)/usb_device
USB_HOST_PATH := $(STM32SDK_DIR)/usb_host
HAL_USB_PATH :=$(STM32SDK_DIR)/hal_usb

SRCS += \
	$(HAL_USB_PATH)/usb_device.c \
	$(HAL_USB_PATH)/usbd_conf.c \
	$(HAL_USB_PATH)/usbd_desc.c \
	$(HAL_USB_PATH)/usbd_hid.c \
	$(HAL_USB_PATH)/usbd_composite.c \
	$(USB_DEVICE_PATH)/Core/Src/usbd_core.c \
	$(USB_DEVICE_PATH)/Core/Src/usbd_ctlreq.c \
	$(USB_DEVICE_PATH)/Core/Src/usbd_ioreq.c

ifeq (yes, $(strip $(USB_HOST_ENABLE)))
SRCS += \
	$(HAL_USB_PATH)/report_parser.c \
	$(HAL_USB_PATH)/usb_host.c \
	$(HAL_USB_PATH)/usbh_conf.c \
	$(HAL_USB_PATH)/usbh_hid.c \
	$(HAL_USB_PATH)/usbh_hid_keybd.c \
	$(HAL_USB_PATH)/usbh_hid_mouse.c \
	$(HAL_USB_PATH)/usbh_hid_parser.c \
	$(USB_HOST_PATH)/Core/Src/usbh_core.c \
	$(USB_HOST_PATH)/Core/Src/usbh_ctlreq.c \
	$(USB_HOST_PATH)/Core/Src/usbh_ioreq.c \
	$(USB_HOST_PATH)/Core/Src/usbh_pipes.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c

APP_DEFS += -DUSB_HOST_ENABLE -DHAL_HCD_MODULE_ENABLED
endif

ifeq (yes, $(strip $(WEBUSB_ENABLE)))
	SRCS += $(HAL_USB_PATH)/usbd_webusb.c
endif

INCS += \
	$(HAL_USB_PATH) \
	$(USB_DEVICE_PATH)/Core/Inc \
	$(USB_HOST_PATH)/Core/Inc \
	$(USB_HOST_PATH)/Class/HID/Inc \
	$(LIB_DIR)/tinyusb/src 
