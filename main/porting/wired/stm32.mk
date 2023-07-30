
PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/stm32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/stm32.c \
	$(PLATFORM_PORTING_DIR)/wait.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \

ifneq (yes, $(TINYUSB_ENABLE))
	ifneq (yes, $(RTOS_ENABLE))
		SRCS += $(PLATFORM_PORTING_DIR)/usb_hal.c
	endif
endif

ifeq (STM32F411, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f411xe.c
endif

ifeq (STM32F401, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f401xc.c
endif

ifeq (STM32F405, $(strip $(MCU)))
	ifeq (yes, $(strip $(USE_F405_V2)))
		SRCS += $(PLATFORM_PORTING_DIR)/stm32f405xx_v2.c
	else
		ifeq (yes, $(strip $(USE_F405_APM)))
		SRCS += $(PLATFORM_PORTING_DIR)/stm32f405xx_apm.c
		else
		SRCS += $(PLATFORM_PORTING_DIR)/stm32f405xx.c
		endif
	endif
endif

ifeq (STM32F446, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f446xx.c
endif

ifeq (STM32F722, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f722xx.c
endif

ifeq (STM32F103, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f103xb.c
endif

ifeq (STM32L072, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32l072xx.c
endif

ifeq (STM32G431, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32g431xx.c
endif

ifneq (,$(findstring $(strip $(MCU)), STM32L432 STM32L452))
	ifneq (yes, $(strip $(LPM_ENABLE)))
		SRCS += $(PLATFORM_PORTING_DIR)/stm32l4xx.c
	else
		SRCS += $(PLATFORM_PORTING_DIR)/stm32l4xx_lpm.c
	endif
endif

ifneq (,$(findstring $(strip $(MCU)), STM32F412))
	SRCS += $(PLATFORM_PORTING_DIR)/stm32f4xx.c
endif