
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32l072xx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_uart_ex.c \

APP_DEFS += \
	-DSTM32L072xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32L0 \

SDK_DEFS += -mcpu=cortex-m0plus -mthumb

MCU_LD = STM32L072CBTx