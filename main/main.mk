
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
	$(MAIN_DIR)/screen \


ifeq (yes, $(strip $(MATRIX_USE_TCA6424)))
	APP_DEFS += -DMATRIX_USE_TCA6424
	SRC_FILES += $(MAIN_DIR)/drivers/tca6424.c
	SRC_FILES += $(MAIN_DIR)/drivers/i2c.c
endif

ifeq (yes, $(strip $(MATRIX_USE_GPIO)))
	APP_DEFS += -DMATRIX_USE_GPIO
endif

ifeq (yes, $(strip $(BOOTMAGIC_ENABLE)))
	ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
		SRC_FILES += $(MAIN_DIR)/porting/nrf52/eeconfig_fds.c
	endif
	ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
		SRC_FILES += $(MAIN_DIR)/porting/eeconfig_mem.c
	endif

	SRC_FILES += $(MAIN_DIR)/eeprom_manager.c
endif

ifeq (yes, $(strip $(SCREEN_ENABLE)))
	LVGL_ENABLE = yes
	APP_DEFS += -DSCREEN_ENABLE
	SRC_FILES += $(MAIN_DIR)/drivers/spi.c
	SRC_FILES += $(MAIN_DIR)/drivers/ssd1357.c
	SRC_FILES += $(MAIN_DIR)/screen/screen.c
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
	ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
		SRC_FILES += $(MAIN_DIR)/matrix_driver.c
	endif
	ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
		SRC_FILES += $(MAIN_DIR)/matrix_amk.c
	endif
endif

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
	include $(MAIN_DIR)/porting/nrf52.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
	SRC_FILES += $(MAIN_DIR)/suspend.c
	SRC_FILES += $(MAIN_DIR)/rtt/SEGGER_RTT.c
	SRC_FILES += $(MAIN_DIR)/rtt/SEGGER_RTT_printf.c
	include $(MAIN_DIR)/porting/stm32.mk
endif