
HAL_USB_PATH :=$(STM32SDK_DIR)/hal_usb

SRCS += \
	$(HAL_USB_PATH)/device/usb_device.c \
	$(HAL_USB_PATH)/device/usbd_conf.c \
	$(HAL_USB_PATH)/device/usbd_desc.c \
	$(HAL_USB_PATH)/device/usbd_composite.c \
	$(HAL_USB_PATH)/device/core/usbd_core.c \
	$(HAL_USB_PATH)/device/core/usbd_ctlreq.c \
	$(HAL_USB_PATH)/device/core/usbd_ioreq.c

ifeq (yes, $(strip $(USB_HOST_ENABLE)))
SRCS += \
	$(HAL_USB_PATH)/host/report_parser.c \
	$(HAL_USB_PATH)/host/usbh_hid_multi.c \
	$(HAL_USB_PATH)/host/usb_host.c \
	$(HAL_USB_PATH)/host/usbh_conf.c \
	$(USB_HOST_PATH)/host/core/usbh_core.c \
	$(USB_HOST_PATH)/host/core/usbh_ctlreq.c \
	$(USB_HOST_PATH)/host/core/usbh_ioreq.c \
	$(USB_HOST_PATH)/host/core/usbh_pipes.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c

INCS += \
	$(HAL_USB_PATH)/host \
	$(HAL_USB_PATH)/host/core \

APP_DEFS += -DUSB_HOST_ENABLE -DHAL_HCD_MODULE_ENABLED
endif

INCS += \
	$(HAL_USB_PATH) \
	$(HAL_USB_PATH)/device \
	$(HAL_USB_PATH)/device/core \
	$(LIB_DIR)/tinyusb/src 
