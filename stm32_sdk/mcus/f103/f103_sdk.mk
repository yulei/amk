SRCS += \
	$(STM32SDK_DIR)/devices/$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f103xb.s \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_uart.c \

APP_DEFS += \
	-DSTM32F103xB \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F1 \

SDK_DEFS += -mcpu=cortex-m3 -mthumb

LINKER_PATH		:= $(STM32SDK_DIR)/linker
LINKER_SCRIPT	:= $(STM32SDK_DIR)/linker/STM32F103CBTx_FLASH.ld