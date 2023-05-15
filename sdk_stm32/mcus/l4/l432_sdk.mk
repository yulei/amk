
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32l432xx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_lptim.c \

APP_DEFS += \
	-DSTM32L432xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32L4 \

ifeq (yes, $(strip $(RTOS_ENABLE)))
	SRCS += $(STM32SDK_DIR)/mcus/$(MCU_SERIES)/stm32l4xx_hal_timebase_tim.c
endif

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32L432KBUx
