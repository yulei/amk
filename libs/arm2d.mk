ARM2D_DIR := $(LIB_DIR)/arm2d

SRCS += \
	$(ARM2D_DIR)/Library/Source/arm_2d_alpha_bleding.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_async.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_conversion.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_draw.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_helium.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_op_table.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_rotation.c \
	$(ARM2D_DIR)/Library/Source/arm_2d_tile.c \
	$(ARM2D_DIR)/Library/Source/arm_2d.c \
	$(ARM2D_DIR)/Helper/Source/arm_2d_helper_pfb.c \

INCS += \
	$(ARM2D_DIR)/Library/Include \
	$(ARM2D_DIR)/Helper/Include \

