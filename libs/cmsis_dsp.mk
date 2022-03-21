DSP_DIR := $(LIB_DIR)/vendor/st/cmsis/DSP

SRCS += \
	$(DSP_DIR)/Source/BasicMathFunctions/BasicMathFunctions.c \
	$(DSP_DIR)/Source/CommonTables/CommonTables.c \
	$(DSP_DIR)/Source/ComplexMathFunctions/ComplexMathFunctions.c \
	$(DSP_DIR)/Source/ControllerFunctions/ControllerFunctions.c \
	$(DSP_DIR)/Source/FastMathFunctions/FastMathFunctions.c \
	$(DSP_DIR)/Source/FilteringFunctions/FilteringFunctions.c \
	$(DSP_DIR)/Source/MatrixFunctions/MatrixFunctions.c \
	$(DSP_DIR)/Source/StatisticsFunctions/StatisticsFunctions.c \
	$(DSP_DIR)/Source/SupportFunctions/SupportFunctions.c \
	$(DSP_DIR)/Source/TransformFunctions/TransformFunctions.c \

INCS += \
	$(DSP_DIR)/Include \

APP_DEFS += -DCMSIS_DSP_ENABLE