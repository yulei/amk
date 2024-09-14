
SRCS += \
	$(STM32SDK_DIR)/mcus/$(MCU_TYPE)/startup_stm32f722xx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2s.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_spi.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_spi_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_iwdg.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_sd.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_sdmmc.c \

ifeq (yes, $(strip $(RTOS_ENABLE)))
	SRCS += $(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_hal_timebase_tim.c
endif

APP_DEFS += \
	-DSTM32F722xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F7 \
	-DSYSTEM_CLOCK=216000000

SDK_DEFS += -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard

MCU_LD = STM32F722RETx