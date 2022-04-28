GD32USB_DIR := ${LIB_DIR}/tinyusb/src

SRCS += \
	${GD32USB_DIR}/class/hid/hid_host.c \
	${GD32USB_DIR}/common/tusb_fifo.c \
	${GD32USB_DIR}/host/usbh_control.c \
	${GD32USB_DIR}/host/usbh.c \
	${GD32USB_DIR}/tusb.c \

INCS += \
	${GD32USB_DIR}/class/hid \
	${GD32USB_DIR}/common \
	${GD32USB_DIR}/host \
	${GD32USB_DIR}/osal \
	${GD32USB_DIR} \

SRCS += \
	${GD32SDK_DIR}/usbfs/drv_usb_core.c \
	${GD32SDK_DIR}/usbfs/drv_usb_host.c \
	${GD32SDK_DIR}/usbfs/drv_usbh_int.c \
	${GD32SDK_DIR}/usbfs/usbh_pipe.c \
	${GD32SDK_DIR}/hcd_gd32.c \

INCS += \
	${GD32SDK_DIR}/usbfs \

APP_DEFS += \
	-DUSE_USB_FS \
	-DUSE_HOST_MODE \
	-DUSB_HOST_ENABLE \
