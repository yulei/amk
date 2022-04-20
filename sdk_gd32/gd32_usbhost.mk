GD32USB_DIR := ${VENDOR_DIR}/${GD32_LIB_DIR}/Firmware/GD32E10x_usbfs_library

SRCS += \
	${GD32USB_DIR}/driver/Source/drv_usb_core.c \
	${GD32USB_DIR}/driver/Source/drv_usb_host.c \
	${GD32USB_DIR}/driver/Source/drv_usbh_int.c \
	${GD32USB_DIR}/host/core/Source/usbh_core.c \
	${GD32USB_DIR}/host/core/Source/usbh_enum.c \
	${GD32USB_DIR}/host/core/Source/usbh_pipe.c \
	${GD32USB_DIR}/host/core/Source/usbh_transc.c \
	${GD32USB_DIR}/host/class/hid/Source/usbh_hid_core.c \
	${GD32USB_DIR}/host/class/hid/Source/usbh_hid_keybd.c \
	${GD32USB_DIR}/host/class/hid/Source/usbh_hid_mouse.c \
	${GD32USB_DIR}/host/class/hid/Source/usbh_hid_parser.c \

INCS += \
	${GD32USB_DIR}/driver/Include \
	${GD32USB_DIR}/host/core/Include \
	${GD32USB_DIR}/host/class/hid/Include \
	${GD32USB_DIR}/ustd/class/hid \
	${GD32USB_DIR}/ustd/common \

APP_DEFS += \
	-DUSE_USB_FS \
	-DUSE_HOST_MODE \
