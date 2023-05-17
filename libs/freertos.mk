FREERTOS_DIR := $(LIB_DIR)/cmsis_freertos

INCS += $(FREERTOS_DIR)/Source/include

SRCS += \
	$(FREERTOS_DIR)/Source/croutine.c \
	$(FREERTOS_DIR)/Source/event_groups.c \
	$(FREERTOS_DIR)/Source/list.c \
	$(FREERTOS_DIR)/Source/queue.c \
	$(FREERTOS_DIR)/Source/stream_buffer.c \
	$(FREERTOS_DIR)/Source/tasks.c \
	$(FREERTOS_DIR)/Source/timers.c \
	$(FREERTOS_DIR)/Source/portable/MemMang/heap_4.c \

ifeq (STM32F722, $(strip $(MCU)))
	INCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM7/r0p1
	SRCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM7/r0p1/port.c
endif

ifneq (,$(filter $(strip $(MCU)),STM32F411 STM32L432))
	INCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/
	SRCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/port.c
endif
