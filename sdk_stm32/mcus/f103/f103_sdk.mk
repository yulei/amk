SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f103xb.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_gpio_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_uart.c \

APP_DEFS += \
	-DSTM32F103xB \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F1 \

SDK_DEFS += -mcpu=cortex-m3 -mthumb

MCU_LD = STM32F103CBTx
