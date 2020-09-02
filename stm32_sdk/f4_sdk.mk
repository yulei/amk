
SRC_FILES += \
	$(STM32SDK_DIR)/startup_stm32f411xe.s \
	$(STM32SDK_DIR)/f4/system_stm32f4xx.c \
	$(STM32SDK_DIR)/f4/stm32f4xx_hal_msp.c \
	$(STM32SDK_DIR)/f4/stm32f4xx_it.c \
	$(STM32SDK_DIR)/f4/usb_device.c \
	$(STM32SDK_DIR)/f4/usbd_desc.c \
	$(STM32SDK_DIR)/f4/usbd_conf.c \
	$(STM32SDK_DIR)/f4/usbd_core.c \
	$(STM32SDK_DIR)/f4/usbd_ctlreq.c \
	$(STM32SDK_DIR)/f4/usbd_ioreq.c \
	$(STM32SDK_DIR)/f4/usbd_hid.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \

INC_FOLDERS += \
	$(STM32SDK_DIR) \
	$(STM32SDK_DIR)/f4 \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc \
	$(STM32SDK_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
	$(STM32SDK_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	$(STM32SDK_DIR)/Drivers/CMSIS/Include \

APP_DEFS += \
	-DUSE_HAL_DRIVER \
	-DSTM32F411xE \

LINKER_PATH		:= $(STM32SDK_DIR)
LINKER_SCRIPT	:= $(STM32SDK_DIR)/STM32F411CEUx_FLASH.ld