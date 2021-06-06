
#always enabled tmk feature
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes

ifeq (yes, $(strip $(MATRIX_USE_TCA6424)))
	APP_DEFS += -DMATRIX_I2C_PINS
	SRCS += $(MAIN_DIR)/drivers/tca6424.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
endif

ifeq (yes, $(strip $(MATRIX_USE_PCA9535)))
	APP_DEFS += -DMATRIX_I2C_PINS
	SRCS += $(MAIN_DIR)/drivers/pca9535.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
endif

ifeq (yes, $(strip $(SCREEN_ENABLE)))
	LVGL_ENABLE = yes
	APP_DEFS += -DSCREEN_ENABLE
	SRCS += $(MAIN_DIR)/drivers/spi.c
	SRCS += $(MAIN_DIR)/drivers/st7735.c
	SRCS += $(MAIN_DIR)/screen/screen.c
	SRCS += $(MAIN_DIR)/screen/fractal.c
endif

ifeq (yes, $(strip $(INDICATOR_ENABLE)))
	SRCS += $(MAIN_DIR)/rgb/rgb_indicator.c
	SRCS += $(MAIN_DIR)/drivers/aw9106b.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DINDICATOR_ENABLE
endif

ifeq (yes, $(strip $(RGB_RING_ENABLE)))
	SRCS += $(MAIN_DIR)/rgb/rgb_ring.c
	SRCS += $(MAIN_DIR)/rgb/rgb_color.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3731.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DRGB_RING_ENABLE
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

ifeq (all, $(strip $(RGB_EFFECTS_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/ws2812.c
	SRCS += $(MAIN_DIR)/drivers/aw9523b.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effects.c
	SRCS += $(MAIN_DIR)/rgb/rgb_color.c
	APP_DEFS += -DRGB_EFFECTS_ENABLE
	APP_DEFS += -DRGB_WITH_ALL
	APP_DEFS += -DRGB_WITH_WS2812
	APP_DEFS += -DRGB_WITH_AW9523B
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
	SRCS += $(MAIN_DIR)/usb/webusb.c
	APP_DEFS += -DWEBUSB_ENABLE
endif

ifeq (yes, $(strip $(MSC_ENABLE)))
	SRCS += $(MAIN_DIR)/screen/anim.c
	SRCS += $(MAIN_DIR)/drivers/w25qxx.c
	SRCS += $(MAIN_DIR)/drivers/spi.c
	SRCS += $(MAIN_DIR)/usb/mscusb.c
	APP_DEFS += -DMSC_ENABLE
endif

ifeq (yes, $(strip $(KEYMAP_CONFIG_ENABLE)))
	APP_DEFS += -DKEYMAP_CONFIG_ENABLE	
endif

ifeq (yes, $(strip $(VIAL_ENABLE)))
	INCS += $(MAIN_DIR)/vial
	SRCS += $(MAIN_DIR)/vial/vial_porting.c
	SRCS += $(MAIN_DIR)/vial/keycode_convert.c
	APP_DEFS += -DVIAL_ENABLE
	ifneq (yes, $(strip $(VIAL_ENABLE)))
		APP_DEFS += -DKEYMAP_CONFIG_ENABLE	
	endif
endif

ifneq (yes, $(strip $(CUSTOM_MATRIX)))
	SRCS += $(MAIN_DIR)/matrix_scan.c
endif
