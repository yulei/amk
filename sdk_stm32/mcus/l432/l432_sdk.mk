
SRCS += \
	$(STM32SDK_DIR)/devices/$(MCU_SERIES)/Source/Templates/gcc/startup_stm32l432xx.s \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \

APP_DEFS += \
	-DSTM32L432xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32L4 \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32L432KBUx