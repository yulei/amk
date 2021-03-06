
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

ifeq (yes, $(strip $(RGB_LINEAR_ENABLE)))
	SRCS += $(MAIN_DIR)/rgb/rgb_linear.c
	APP_DEFS += -DRGB_LINEAR_ENABLE
	RGB_ENABLE = yes
endif

ifeq (yes, $(strip $(RGB_INDICATOR_ENABLE)))
	SRCS += $(MAIN_DIR)/rgb/rgb_indicator.c
	APP_DEFS += -DRGB_INDICATOR_ENABLE
	RGB_ENABLE = yes
endif

ifeq (yes, $(strip $(RGB_MATRIX_ENABLE)))
	SRCS += $(MAIN_DIR)/rgb/rgb_matrix.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effect_matrix.c
	APP_DEFS += -DRGB_MATRIX_ENABLE
	RGB_ENABLE = yes
endif

ifeq (yes, $(strip $(RGB_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/aw9106b.c
	SRCS += $(MAIN_DIR)/drivers/aw9523b.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3236.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3731.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3733.c
	SRCS += $(MAIN_DIR)/drivers/ws2812.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_common.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effect_linear.c
	SRCS += $(MAIN_DIR)/rgb/rgb_led.c
	APP_DEFS += -DRGB_ENABLE
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
	SRCS += $(MAIN_DIR)/amk/matrix_scan.c
endif

ifeq (yes, $(EECONFIG_FRAM)) 
	SRCS += $(MAIN_DIR)/amk/eeconfig_fram.c
	SRCS += $(MAIN_DIR)/drivers/mb85rcxx.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DEECONFIG_FRAM
else
	ifeq (yes, $(EECONFIG_FLASH))
		SRCS += $(PLATFORM_PORTING_DIR)/eeconfig_flash.c
		APP_DEFS += -DEECONFIG_FLASH
	else
		SRCS += $(MAIN_DIR)/amk/eeconfig_mem.c
	endif
endif