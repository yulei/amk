GD32USB_DIR := ${LIB_DIR}/tinyusb/src

SRCS += \
	${GD32USB_DIR}/common/tusb_fifo.c \
	${GD32USB_DIR}/tusb.c \
	${GD32SDK_DIR}/usbfs/drv_usb_core.c \

INCS += \
	${GD32USB_DIR}/common \
	${GD32USB_DIR}/osal \
	${GD32SDK_DIR}/usbfs \
	${GD32SDK_DIR} \

APP_DEFS += \
	-DUSE_USB_FS \

ifeq (yes, $(strip $(USB_HOST_ENABLE)))
	SRCS += ${GD32SDK_DIR}/usbfs/drv_usbh_int.c
	SRCS += ${GD32SDK_DIR}/usbfs/usbh_pipe.c
	SRCS += ${GD32SDK_DIR}/usbfs/drv_usb_host.c
	SRCS += ${GD32SDK_DIR}/hcd_gd32.c
	SRCS += ${GD32USB_DIR}/class/hid/hid_host.c
	SRCS += ${GD32USB_DIR}/host/usbh_control.c
	SRCS += ${GD32USB_DIR}/host/usbh.c
	INCS += ${GD32USB_DIR}/class/hid 
	INCS += ${GD32USB_DIR}/host
	APP_DEFS += -DUSE_HOST_MODE
	APP_DEFS += -DUSB_HOST_ENABLE
endif

ifeq (yes, $(strip $(USB_DEVICE_ENABLE)))
	SRCS += ${GD32SDK_DIR}/usbfs/drv_usbd_int.c
	SRCS += ${GD32SDK_DIR}/usbfs/drv_usb_dev.c
	SRCS += ${GD32SDK_DIR}/dcd_gd32.c
	SRCS += ${GD32USB_DIR}/class/hid/hid_device.c
	SRCS += ${GD32USB_DIR}/class/audio/audio_device.c
	SRCS += ${GD32USB_DIR}/class/msc/msc_device.c
	SRCS += ${GD32USB_DIR}/device/usbd_control.c
	SRCS += ${GD32USB_DIR}/device/usbd.c
	INCS += ${GD32USB_DIR}/device/hid 
	INCS += ${GD32USB_DIR}/device
	APP_DEFS += -DUSE_DEVICE_MODE
	APP_DEFS += -DUSB_DEVICE_ENABLE
endif