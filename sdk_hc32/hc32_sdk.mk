HC32SDK_DIR := sdk_hc32

ifeq (HC32F460, $(strip $(MCU)))
MCU_SERIES := hc32 
MCU_TYPE := f460
HC32_DDL := hc32f460_ddl_Rev2.2.0
endif

SRCS += \
	$(VENDOR_DIR)/$(HC32_DDL)/mcu/GCC/Startup/startup_hc32f460.S \
	$(VENDOR_DIR)/$(HC32_DDL)/mcu/common/system_hc32f460.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_adc.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_emb.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_keyscan.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_sram.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timera.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_aes.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_event_port.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_mpu.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_swdt.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_trng.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_can.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_exint_nmi_swi.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_ots.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer0.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_usart.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_clk.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_gpio.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_pwc.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer4_cnt.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_utility.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_cmp.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_hash.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_qspi.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer4_emb.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_wdt.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_crc.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_i2c.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_rmu.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer4_oco.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_dcu.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_i2s.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_rtc.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer4_pwm.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_dmac.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_icg.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_sdioc.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer4_sevt.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_efm.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_interrupts.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_spi.c \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/src/hc32f460_timer6.c \

INCS += \
	$(VENDOR_DIR)/$(HC32_DDL)/mcu/common \
	$(VENDOR_DIR)/$(HC32_DDL)/mcu/GCC/CMSIS/Core/Include \
	$(VENDOR_DIR)/$(HC32_DDL)/driver/inc \

APP_DEFS += \
	-DUSE_DEVICE_DRIVER_LIB \

include $(HC32SDK_DIR)/$(MCU_TYPE)_sdk.mk

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
endif

include $(HC32SDK_DIR)/dcd_usb.mk

LINKER_PATH	?= $(VENDOR_DIR)/$(HC32_DDL)/mcu/GCC/LinkerScript
LINKER_SCRIPT := $(LINKER_PATH)/$(MCU_LD)_flash.ld
