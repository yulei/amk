MCU_FAMILY := stm32f1xx
MCU_SRC_PATH := f1
MCU_HAL_PATH := STM32F1xx_HAL_Driver

SRC_FILES += \
	$(STM32SDK_DIR)/startup_stm32f103xb.s \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/system_$(MCU_FAMILY).c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/$(MCU_FAMILY)_hal_msp.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/$(MCU_FAMILY)_it.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usb_device.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_desc.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_conf.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_core.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_ctlreq.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_ioreq.c \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH)/usbd_hid.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pcd.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pcd_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_ll_usb.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_tim.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_rcc.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_rcc_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_flash.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_flash_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_gpio.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_gpio_ex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_dma.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_pwr.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_cortex.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_exti.c \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Src/$(MCU_FAMILY)_hal_uart.c \

INC_FOLDERS += \
	$(STM32SDK_DIR) \
	$(STM32SDK_DIR)/$(MCU_SRC_PATH) \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Inc \
	$(STM32SDK_DIR)/Drivers/$(MCU_HAL_PATH)/Inc/Legacy \
	$(STM32SDK_DIR)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
	$(STM32SDK_DIR)/Drivers/CMSIS/Include \

APP_DEFS += \
	-DUSE_HAL_DRIVER \
	-DSTM32F103xB \

SDK_DEFS += -mcpu=cortex-m3 -mthumb

LINKER_PATH		:= $(STM32SDK_DIR)
LINKER_SCRIPT	:= $(STM32SDK_DIR)/STM32F103CBTx_FLASH.ld