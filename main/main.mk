
#always enabled tmk feature
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes

SRCS += \
	$(MAIN_DIR)/main.c  \
	$(MAIN_DIR)/custom_action.c \
	$(MAIN_DIR)/amk_keymap.c \
	$(MAIN_DIR)/usb_descriptors.c \
	$(MAIN_DIR)/amk_printf.c \
	$(MAIN_DIR)/eeprom_manager.c \
	$(MAIN_DIR)/amk_boot.c \
	$(MAIN_DIR)/report_queue.c \
	$(MAIN_DIR)/rgb/rgb_led.c \

INCS += \
	$(MAIN_DIR) \
	$(MAIN_DIR)/porting \
	$(MAIN_DIR)/drivers \
	$(MAIN_DIR)/rgb \
	$(MAIN_DIR)/screen \
	$(MAIN_DIR)/rtt \
	$(LIB_DIR)/tinyusb/src \


ifeq (yes, $(strip $(MATRIX_USE_TCA6424)))
	APP_DEFS += -DMATRIX_USE_TCA6424
	SRCS += $(MAIN_DIR)/drivers/tca6424.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
endif

ifeq (yes, $(strip $(SCREEN_ENABLE)))
	LVGL_ENABLE = yes
	APP_DEFS += -DSCREEN_ENABLE
	SRCS += $(MAIN_DIR)/drivers/spi.c
	SRCS += $(MAIN_DIR)/drivers/ssd1357.c
	SRCS += $(MAIN_DIR)/screen/screen.c
	SRCS += $(MAIN_DIR)/screen/gifdec.c
	SRCS += $(MAIN_DIR)/screen/image.c
	SRCS += $(MAIN_DIR)/screen/upng.c
endif

ifeq (yes, $(strip $(RGB_EFFECTS_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/aw9523b.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effects.c
	SRCS += $(MAIN_DIR)/rgb/rgb_color.c
	APP_DEFS += -DRGB_EFFECTS_ENABLE
	APP_DEFS += -DRGB_WITH_AW9523B
endif

ifeq (ws2812, $(strip $(RGB_EFFECTS_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/ws2812.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effects.c
	SRCS += $(MAIN_DIR)/rgb/rgb_color.c
	APP_DEFS += -DRGB_EFFECTS_ENABLE
	APP_DEFS += -DRGB_WITH_WS2812
endif

ifeq (yes, $(strip $(RGB_MATRIX_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/is31fl3731.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_matrix.c
	SRCS += $(MAIN_DIR)/rgb/rgb_color.c
	APP_DEFS += -DRGB_MATRIX_ENABLE
	APP_DEFS += -DRGB_WITH_IS31FL3731
endif

ifeq (is31fl3733, $(strip $(RGB_MATRIX_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/is31fl3733.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_matrix.c
	SRCS += $(MAIN_DIR)/rgb/rgb_color.c
	APP_DEFS += -DRGB_MATRIX_ENABLE
	APP_DEFS += -DRGB_WITH_IS31FL3733
endif

ifeq (yes, $(strip $(WEBUSB_ENABLE)))
	SRCS += $(MAIN_DIR)/webusb.c
	APP_DEFS += -DWEBUSB_ENABLE
endif

ifneq (yes, $(strip $(CUSTOM_MATRIX)))
	SRCS += $(MAIN_DIR)/matrix_scan.c
endif

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
	include $(MAIN_DIR)/porting/nrf52.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
	SRCS += $(MAIN_DIR)/suspend.c
	SRCS += $(MAIN_DIR)/rtt/SEGGER_RTT.c
	include $(MAIN_DIR)/porting/stm32.mk
endif
