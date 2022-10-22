FREERTOS_DIR := $(LIB_DIR)/cmsis_freertos

INCS += \
	$(FREERTOS_DIR)/Source/include \
	$(FREERTOS_DIR)/CMSIS/RTOS2/FreeRTOS/Include \

SRCS += \
	$(FREERTOS_DIR)/Source/tasks.c \
	$(FREERTOS_DIR)/Source/list.c \
	$(FREERTOS_DIR)/Source/queue.c \
	$(FREERTOS_DIR)/Source/timers.c \
	$(FREERTOS_DIR)/Source/event_groups.c \
	$(FREERTOS_DIR)/Source/portable/MemMang/heap_4.c \
	$(FREERTOS_DIR)/CMSIS/RTOS2/FreeRTOS/Source/ARM/clib_arm.c \
	$(FREERTOS_DIR)/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c \
	$(FREERTOS_DIR)/CMSIS/RTOS2/FreeRTOS/Source/freertos_evr.c \
#	$(FREERTOS_DIR)/CMSIS/RTOS2/FreeRTOS/Source/os_systick.c \
#	$(FREERTOS_DIR)/CMSIS/RTOS2/FreeRTOS/Source/handlers.c \
	$(FREERTOS_DIR)/Source/croutine.c \
	$(FREERTOS_DIR)/Source/stream_buffer.c \

ifeq (STM32F722, $(strip $(MCU)))
	INCS += $(FREERTOS_DIR)/kernel/portable/GCC/ARM_CM7/r0p1
	SRCS += $(FREERTOS_DIR)/kernel/portable/GCC/ARM_CM7/r0p1/port.c
endif

ifneq (,$(filter $(strip $(MCU)),STM32F411 STM32L432))
	INCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/
	SRCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F/port.c
endif

ifeq (GD32E505, $(strip $(MCU)))
	INCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM33_NTZ/non_secure
	SRCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
	SRCS += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
endif