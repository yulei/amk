
MCU_FAMILY := stm32f4xx
MCU_SRC_PATH := f405
MCU_HAL_PATH := STM32F4xx_HAL_Driver

SRC_FILES += \
	$(STM32SDK_DIR)/startup_stm32f405xx.s \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/system_$(MCU_FAMILY).c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/$(MCU_FAMILY)_hal_msp.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/$(MCU_FAMILY)_it.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usb_device.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_conf.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_core.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_ctlreq.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_desc.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_desc.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_hid.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_composite.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_webusb.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_ioreq.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usb_host.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_conf.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_core.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_ctlreq.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_hid.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_hid_keybd.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_hid_mouse.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_hid_parser.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_ioreq.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbh_pipes.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pcd.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pcd_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_ll_usb.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_tim.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_rcc.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_rcc_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_flash.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_flash_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_gpio.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_dma.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pwr.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_cortex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_exti.c \

INC_FOLDERS += \
	$(STM32SDK_DIR) \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH) \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Inc \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Inc/Legacy \
	$(STM32SDK_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	$(STM32SDK_DIR)/Drivers/CMSIS/Include \

APP_DEFS += \
	-DUSE_HAL_DRIVER \
	-DSTM32F405xx \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

LINKER_PATH		:= $(STM32SDK_DIR)
LINKER_SCRIPT	:= $(STM32SDK_DIR)/STM32F405RGTx_FLASH.ld