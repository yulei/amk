
USB_SRC_PATH := f4_usb

SRC_FILES += \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usb_device.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_conf.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_core.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_ctlreq.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_desc.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_desc.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_hid.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_composite.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_webusb.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbd_ioreq.c \

INC_FOLDERS += \
	$(STM32SDK_DIR)/$(USB_SRC_PATH) \

ifeq (STM32F405, $(strip $(MCU)))
SRC_FILES += \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usb_host.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_conf.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_core.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_ctlreq.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_hid.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_hid_keybd.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_hid_mouse.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_hid_parser.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_ioreq.c \
	$(STM32SDK_DIR)/$(USB_SRC_PATH)/usbh_pipes.c 
endif