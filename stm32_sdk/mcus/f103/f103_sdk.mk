SRCS += \
	$(STM32SDK_DIR)/cmsis_device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f103xb.s \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_rcc_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio_ex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_cortex.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_exti.c \
	$(STM32SDK_DIR)/drivers/$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_uart.c \

APP_DEFS += \
	-DSTM32F103xB \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F1 \

SDK_DEFS += -mcpu=cortex-m3 -mthumb

LINKER_PATH		:= $(STM32SDK_DIR)/linker
LINKER_SCRIPT	:= $(STM32SDK_DIR)/linker/STM32F103CBTx_FLASH.ld