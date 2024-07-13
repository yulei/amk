
HAL_USB_PATH :=$(STM32SDK_DIR)/hal_usb

SRCS += \
	$(HAL_USB_PATH)/host/hal_host.c \
	$(HAL_USB_PATH)/host/usb_host.c \
	$(HAL_USB_PATH)/host/report_parser.c \
	$(HAL_USB_PATH)/host/usbh_hid_multi.c \
	$(HAL_USB_PATH)/host/core/usbh_core.c \
	$(HAL_USB_PATH)/host/core/usbh_ctlreq.c \
	$(HAL_USB_PATH)/host/core/usbh_ioreq.c \
	$(HAL_USB_PATH)/host/core/usbh_pipes.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c

#APP_DEFS += -DUSB_HOST_ENABLE -DHAL_HCD_MODULE_ENABLED -DHAL_HOST_ENABLE
APP_DEFS += -DHAL_HCD_MODULE_ENABLED -DHAL_HOST_ENABLE

INCS += \
	$(HAL_USB_PATH) \
	$(HAL_USB_PATH)/host \
	$(HAL_USB_PATH)/host/core \

ifeq (yes, $(strip $(HOST_MSC_ENABLE)))
SRCS += $(HAL_USB_PATH)/host/usbh_msc_bot.c
SRCS += $(HAL_USB_PATH)/host/usbh_msc_scsi.c
SRCS += $(HAL_USB_PATH)/host/usbh_msc.c
endif