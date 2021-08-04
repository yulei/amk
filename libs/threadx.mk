THREADX_DIR := $(LIB_DIR)/rtos/threadx

SRCS += $(wildcard $(THREADX_DIR)/common/src/*.c)

INCS += $(THREADX_DIR)/common/inc

ifeq (STM32F722, $(strip $(MCU)))
INCS += $(THREADX_DIR)/ports/cortex_m7/gnu/inc
SRCS += $(wildcard $(THREADX_DIR)/ports/cortex_m7/gnu/src/*.S)
endif