

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
	APP_DEFS += -DSCREEN_ENABLE
	SRCS += $(MAIN_DIR)/drivers/spi.c
	#SRCS += $(MAIN_DIR)/drivers/spi_lcd.c
	SRCS += $(MAIN_DIR)/drivers/screen_driver.c

	ifeq (RM67160, $(strip $(SCREEN_DRIVER)))
		APP_DEFS += -DSCREEN_DRIVER_RM67160
		SRCS += $(MAIN_DIR)/drivers/rm67160.c
	else
		ifeq (ST7789, $(strip $(SCREEN_DRIVER)))
			APP_DEFS += -DSCREEN_DRIVER_ST7789
			SRCS += $(MAIN_DIR)/drivers/st7789.c
		else
			ifeq (SSD1306, $(strip $(SCREEN_DRIVER)))
				APP_DEFS += -DSCREEN_DRIVER_SSD1306
				SRCS += $(MAIN_DIR)/drivers/ssd1306.c
			else
				ifeq (GC9107, $(strip $(SCREEN_DRIVER)))
					APP_DEFS += -DSCREEN_DRIVER_GC9107
					SRCS += $(MAIN_DIR)/drivers/gc9107.c
				else
					APP_DEFS += -DSCREEN_DRIVER_ST7735
					SRCS += $(MAIN_DIR)/drivers/st7735.c
				endif
			endif
		endif
	endif
	SRCS += $(MAIN_DIR)/screen/render.c
	SRCS += $(MAIN_DIR)/screen/screen.c
	SRCS += $(MAIN_DIR)/screen/display.c
	SRCS += $(MAIN_DIR)/screen/anim_display.c
	SRCS += $(MAIN_DIR)/screen/audio_display.c
	SRCS += $(MAIN_DIR)/screen/keyboard_display.c
	SRCS += $(MAIN_DIR)/screen/info_display.c
	SRCS += $(MAIN_DIR)/screen/usage_table.c
endif

ifeq (yes, $(strip $(RGB_LINEAR_ENABLE)))
	SRCS += $(MAIN_DIR)/rgb/rgb_linear.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effect_linear.c
#	SRCS += $(MAIN_DIR)/rgb/rgb_effect_linear_qmk.c
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
	#SRCS += $(MAIN_DIR)/rgb/rgb_effect_matrix.c
	SRCS += $(MAIN_DIR)/rgb/rgb_effect_matrix_qmk.c
	SRCS += $(MAIN_DIR)/rgb/qmk/lib8tion/lib8tion.c
	APP_DEFS += -DAMK_RGB_MATRIX_ENABLE
	RGB_ENABLE = yes
endif

ifeq (yes, $(strip $(RGB_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/aw9106b.c
	SRCS += $(MAIN_DIR)/drivers/aw9523b.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3236.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3731.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3733.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3741.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3746.c
	SRCS += $(MAIN_DIR)/drivers/is31fl3729.c
	SRCS += $(MAIN_DIR)/drivers/ws2812.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/rgb_driver.c
	SRCS += $(MAIN_DIR)/rgb/rgb_common.c
	SRCS += $(MAIN_DIR)/rgb/rgb_led.c
	APP_DEFS += -DRGB_ENABLE
endif

ifeq (yes, $(strip $(MSC_ENABLE)))
	SRCS += $(MAIN_DIR)/screen/anim.c
	SRCS += $(MAIN_DIR)/usb/mscusb.c

	ifeq (yes, $(strip $(QSPI_ENABLE)))
		SRCS += $(MAIN_DIR)/drivers/qspi.c
		APP_DEFS += -DQSPI_ENABLE
	else
		SRCS += $(MAIN_DIR)/drivers/w25qxx.c
		SRCS += $(MAIN_DIR)/drivers/spi.c
	endif
	APP_DEFS += -DMSC_ENABLE
endif

ifeq (yes, $(strip $(AUDIO_ENABLE)))
	SRCS += $(MAIN_DIR)/usb/audiousb.c
	APP_DEFS += -DAUDIO_ENABLE
endif

ifeq (yes, $(strip $(VIAL_ENABLE)))
#	INCS += $(MAIN_DIR)/vial
#	SRCS += $(MAIN_DIR)/vial/vial_porting.c
#	SRCS += $(MAIN_DIR)/vial/vial_macro.c
#	SRCS += $(MAIN_DIR)/vial/keycode_convert.c
#	SRCS += $(MAIN_DIR)/vial/send_string.c
#	APP_DEFS += -DVIAL_ENABLE
#	APP_DEFS += -DKEYMAP_CONFIG_ENABLE	
#	ACTIONMAP_ENABLE := yes
endif

ifneq (yes, $(strip $(CUSTOM_MATRIX)))
#	SRCS += $(MAIN_DIR)/amk/matrix_scan.c
#	SRCS += $(MAIN_DIR)/amk/debounce/debounce.c
#	INCS += $(MAIN_DIR)/amk/debounce
endif

ifeq (yes, $(EECONFIG_FRAM)) 
	SRCS += $(MAIN_DIR)/amk/eeconfig_fram.c
	SRCS += $(MAIN_DIR)/drivers/mb85rcxx.c
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DEECONFIG_FRAM
else
	ifeq (yes, $(EECONFIG_FLASH))
		SRCS += $(MAIN_DIR)/amk/eeconfig_flash.c
		SRCS += $(MAIN_DIR)/drivers/flash.c
		APP_DEFS += -DEECONFIG_FLASH
	else
		SRCS += $(MAIN_DIR)/amk/eeconfig_mem.c
	endif
endif

ifeq (yes, $(strip $(DATETIME_ENABLE)))
	ifeq (bl5372, $(strip $(RTC_DRIVER)))
	SRCS += $(MAIN_DIR)/drivers/bl5372.c
	else
	SRCS += $(MAIN_DIR)/drivers/rtc8563.c
	endif

	SRCS += $(MAIN_DIR)/drivers/i2c.c
	APP_DEFS += -DDATETIME_ENABLE
endif

ifeq (yes, $(strip $(NOFRENDO_ENABLE)))
	include libs/nofrendo.mk
endif

ifeq (yes, $(strip $(DYNAMIC_CONFIGURATION)))
	APP_DEFS += -DDYNAMIC_CONFIGURATION
endif

ifeq (yes, $(strip $(INA219_ENABLE)))
	SRCS += $(MAIN_DIR)/drivers/i2c.c
	SRCS += $(MAIN_DIR)/drivers/ina219.c
	APP_DEFS += -DINA219_ENABLE
endif

ifeq (yes, $(strip $(MATRIX_EC_ENABLE)))
	SRCS += $(MAIN_DIR)/amk/ec_matrix.c
	APP_DEFS += -DMATRIX_EC
endif

ifeq (yes,$(strip $(NKRO_ENABLE)))
	APP_DEFS += -DNKRO_ENABLE
endif
