CMSIS_OS_DIR := $(LIB_DIR)/cmsis_5/CMSIS/RTOS2

INCS += \
	$(CMSIS_OS_DIR)/Include \
	$(CMSIS_OS_DIR)/RTX/Include \
	$(CMSIS_OS_DIR)/RTX/Source \

SRCS += \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_delay.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_evflags.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_evr.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_kernel.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_lib.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_memory.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_mempool.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_msgqueue.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_mutex.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_semaphore.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_system.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_thread.c \
	$(CMSIS_OS_DIR)/RTX/Source/rtx_timer.c \
	$(CMSIS_OS_DIR)/Source/os_systick.c \
	$(CMSIS_OS_DIR)/Config/handlers.c \
	$(CMSIS_OS_DIR)/Config/RTX_Config.c \

ifeq (CORTEX_M4, $(strip $(MCU_ARCH)))
	SRCS += $(CMSIS_OS_DIR)/RTX/Source/GCC/irq_armv7m.S
endif

ifeq (CORTEX_M7, $(strip $(MCU_ARCH)))
	SRCS += $(CMSIS_OS_DIR)/RTX/Source/GCC/irq_armv7m.S
endif

ifeq (CORTEX_M33, $(strip $(MCU_ARCH)))
	SRCS += $(CMSIS_OS_DIR)/RTX/Source/GCC/irq_armv8mml.S
endif
