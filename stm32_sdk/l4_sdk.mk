
SRC_FILES += \
	$(STM32SDK_DIR)/startup_stm32l433xx.s \
	$(STM32SDK_DIR)/l4/system_stm32l4xx.c \
	$(STM32SDK_DIR)/l4/stm32l4xx_hal_msp.c \
	$(STM32SDK_DIR)/l4/stm32l4xx_it.c \
	$(STM32SDK_DIR)/l4/usb_device.c \
	$(STM32SDK_DIR)/l4/usbd_desc.c \
	$(STM32SDK_DIR)/l4/usbd_conf.c \
	$(STM32SDK_DIR)/l4/usbd_core.c \
	$(STM32SDK_DIR)/l4/usbd_ctlreq.c \
	$(STM32SDK_DIR)/l4/usbd_ioreq.c \
	$(STM32SDK_DIR)/l4/usbd_hid.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \

INC_FOLDERS += \
	$(STM32SDK_DIR) \
	$(STM32SDK_DIR)/l4 \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Inc \
	$(STM32SDK_DIR)/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
	$(STM32SDK_DIR)/Drivers/CMSIS/Device/ST/STM32L4xx/Include \
	$(STM32SDK_DIR)/Drivers/CMSIS/Include \

APP_DEFS += \
	-DUSE_HAL_DRIVER \
	-DSTM32L433xx \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

LINKER_PATH		:= $(STM32SDK_DIR)
LINKER_SCRIPT	:= $(STM32SDK_DIR)/STM32L433RCTx_FLASH.ld