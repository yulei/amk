FREERTOS_DIR := $(LIB_DIR)/rtos/freertos

INCS += $(FREERTOS_DIR) \
	$(FREERTOS_DIR)/kernel/include

SRCS += \
	$(FREERTOS_DIR)/cmsis_os2.c \
	$(FREERTOS_DIR)/kernel/croutine.c \
	$(FREERTOS_DIR)/kernel/event_groups.c \
	$(FREERTOS_DIR)/kernel/list.c \
	$(FREERTOS_DIR)/kernel/queue.c \
	$(FREERTOS_DIR)/kernel/stream_buffer.c \
	$(FREERTOS_DIR)/kernel/tasks.c \
	$(FREERTOS_DIR)/kernel/timers.c \
	$(FREERTOS_DIR)/kernel/portable/MemMang/heap_4.c \

ifeq (STM32F722, $(strip $(MCU)))
	INCS += $(FREERTOS_DIR)/kernel/portable/GCC/ARM_CM7/r0p1
	SRCS += $(FREERTOS_DIR)/kernel/portable/GCC/ARM_CM7/r0p1/port.c
endif

ifeq (STM32F411, $(strip $(MCU)))
	INCS += $(FREERTOS_DIR)/kernel/portable/GCC/ARM_CM4F/
	SRCS += $(FREERTOS_DIR)/kernel/portable/GCC/ARM_CM4F/port.c
endif