
MAIN_DIR := $(TOP_DIR)/main

SRC_FILES += \
	$(MAIN_DIR)/main.c  \
	$(MAIN_DIR)/xprintf.c \
	$(MAIN_DIR)/custom_action.c \

INC_FOLDERS += \
	$(MAIN_DIR) \
	$(MAIN_DIR)/rtt \
	$(MAIN_DIR)/porting \
	$(MAIN_DIR)/drivers \
	$(MAIN_DIR)/rgb \


ifeq (yes, $(strip $(MATRIX_USE_TCA6424)))
	APP_DEFS += -DMATRIX_USE_TCA6424
	SRC_FILES += $(MAIN_DIR)/drivers/tca6424.c
	SRC_FILES += $(MAIN_DIR)/drivers/i2c.c
endif

ifeq (yes, $(strip $(MATRIX_USE_GPIO)))
	APP_DEFS += -DMATRIX_USE_GPIO
endif

ifeq (yes, $(strip $(BOOTMAGIC_ENABLE)))
	ifeq (NRF52832, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/porting/nrf52/eeconfig_fds.c
	endif
	ifeq (NRF52840, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/porting/nrf52/eeconfig_fds.c
	endif
	ifeq (STM32F411, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/porting/eeconfig_mem.c
	endif
	ifeq (STM32L433, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/porting/eeconfig_mem.c
	endif
	SRC_FILES += $(MAIN_DIR)/eeprom_manager.c
endif

ifeq (yes, $(strip $(RGB_EFFECTS_ENABLE)))
	SRC_FILES += $(MAIN_DIR)/drivers/aw9523b.c
	SRC_FILES += $(MAIN_DIR)/drivers/ws2812.c
	SRC_FILES += $(MAIN_DIR)/drivers/i2c.c
	SRC_FILES += $(MAIN_DIR)/rgb/rgb_effects.c
	SRC_FILES += $(MAIN_DIR)/rgb/rgb_driver.c
	SRC_FILES += $(MAIN_DIR)/rgb/rgb_color.c
	APP_DEFS += -DRGB_EFFECTS_ENABLE
endif

ifneq (yes, $(strip $(CUSTOM_MATRIX)))
	ifeq (NRF52832, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/matrix_driver.c
	endif
	ifeq (NRF52840, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/matrix_driver.c
	endif
	ifeq (STM32F411, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/matrix_amk.c
	endif
	ifeq (STM32L433, $(strip $(MCU)))
		SRC_FILES += $(MAIN_DIR)/matrix_amk.c
	endif
endif

ifneq (,$(filter $(strip $(MCU)),NRF52832 NRF52840))
	include $(MAIN_DIR)/porting/nrf52.mk
endif

ifeq (STM32F411, $(strip $(MCU)))
	SRC_FILES += $(MAIN_DIR)/suspend.c
	SRC_FILES += $(MAIN_DIR)/rtt/SEGGER_RTT.c
	SRC_FILES += $(MAIN_DIR)/rtt/SEGGER_RTT_printf.c
	include $(MAIN_DIR)/porting/stm32.mk
endif

ifeq (STM32L433, $(strip $(MCU)))
	SRC_FILES += $(MAIN_DIR)/suspend.c
	SRC_FILES += $(MAIN_DIR)/rtt/SEGGER_RTT.c
	SRC_FILES += $(MAIN_DIR)/rtt/SEGGER_RTT_printf.c
	include $(MAIN_DIR)/porting/stm32.mk
endif