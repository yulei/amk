
SRCS += \
	$(STM32SDK_DIR)/devices/$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f405xx.s \
	$(STM32SDK_DIR)/$(MCU_PATH)/system_$(MCU_FAMILY).c \
	$(STM32SDK_DIR)/$(MCU_PATH)/$(MCU_FAMILY)_hal_msp.c \
	$(STM32SDK_DIR)/$(MCU_PATH)/$(MCU_FAMILY)_it.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_hcd.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_cortex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_exti.c \

APP_DEFS += \
	-DSTM32F405xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F4 \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

LINKER_PATH		:= $(STM32SDK_DIR)/linker
LINKER_SCRIPT	:= $(STM32SDK_DIR)/linker/STM32F405RGTx_FLASH.ld