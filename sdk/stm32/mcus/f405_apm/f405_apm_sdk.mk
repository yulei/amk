
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f405xx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_tim_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_spi.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_sd.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_mmc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_sdmmc.c \

ifeq (yes, $(strip $(RTOS_ENABLE)))
	SRCS += $(STM32SDK_DIR)/mcus/$(MCU_TYPE)/$(MCU_FAMILY)_hal_timebase_tim.c
#else
#	SRCS += $(LIB_DIR)/vendor/geehy/APM32F4xx_StdPeriphDriver/src/apm32f4xx_misc.c
#	SRCS += $(LIB_DIR)/vendor/geehy/APM32F4xx_StdPeriphDriver/src/apm32f4xx_rcm.c
#	SRCS += $(LIB_DIR)/vendor/geehy/APM32F4xx_StdPeriphDriver/src/apm32f4xx_usb_device.c
#	SRCS += $(LIB_DIR)/vendor/geehy/APM32F4xx_StdPeriphDriver/src/apm32f4xx_usb.c

#	INCS += $(LIB_DIR)/vendor/geehy/APM32F4xx/Include
#	INCS += $(LIB_DIR)/vendor/geehy/APM32F4xx_StdPeriphDriver/inc
endif

APP_DEFS += \
	-DCFG_TUSB_MCU=OPT_MCU_APM32F40X \
    -DAPM32F407 \
    -DAPM32F40X \
	-DBOARD_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED \
	-DSTM32F405xx \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32F405RGTx

#XXX
#APP_DEFS += -fno-inline-functions-called-once
#APP_DEFS += -fno-inline-small-functions