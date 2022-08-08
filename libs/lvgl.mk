LVGL_DIR := $(LIB_DIR)
LVGL_DIR_NAME := lvgl

#extra
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/extra/lv_extra.mk

# core
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/core/lv_core.mk

#draw
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/lv_draw.mk

#draw software
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_arc.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_blend.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_img.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_letter.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_line.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_rect.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_polygon.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_gradient.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw/lv_draw_sw_dither.c

#font
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/font/lv_font.mk

#hal
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal/lv_hal_disp.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal/lv_hal_indev.c
SRCS += $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal/lv_hal_tick.c

#misc
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/misc/lv_misc.mk

#widgets
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/widgets/lv_widgets.mk

INCS += \
	$(LVGL_DIR) \
	$(LVGL_DIR)/src/core \
	$(LVGL_DIR)/src/draw \
	$(LVGL_DIR)/src/draw/sw \
	$(LVGL_DIR)/src/font \
	$(LVGL_DIR)/src/hal \
	$(LVGL_DIR)/src/misc \
	$(LVGL_DIR)/src/widgets \

APP_DEFS += \
	-DLV_CONF_INCLUDE_SIMPLE
