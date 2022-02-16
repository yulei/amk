LVGL_DIR := $(LIB_DIR)/lvgl

#extra
SRCS += $(shell find -L $(LVGL_DIR)/src/extra -name \*.c)

# core
SRCS += $(LVGL_DIR)/src/core/lv_disp.c
SRCS += $(LVGL_DIR)/src/core/lv_group.c
SRCS += $(LVGL_DIR)/src/core/lv_indev.c
SRCS += $(LVGL_DIR)/src/core/lv_indev_scroll.c
SRCS += $(LVGL_DIR)/src/core/lv_obj.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_class.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_draw.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_pos.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_scroll.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_style.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_style_gen.c
SRCS += $(LVGL_DIR)/src/core/lv_obj_tree.c
SRCS += $(LVGL_DIR)/src/core/lv_event.c
SRCS += $(LVGL_DIR)/src/core/lv_refr.c
SRCS += $(LVGL_DIR)/src/core/lv_theme.c

#draw
SRCS += $(LVGL_DIR)/src/draw/lv_draw_arc.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw_img.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw_label.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw_line.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw_mask.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw_rect.c
SRCS += $(LVGL_DIR)/src/draw/lv_draw_triangle.c
SRCS += $(LVGL_DIR)/src/draw/lv_img_buf.c
SRCS += $(LVGL_DIR)/src/draw/lv_img_cache.c
SRCS += $(LVGL_DIR)/src/draw/lv_img_decoder.c

#draw software
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_arc.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_blend.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_img.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_letter.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_line.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_rect.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_polygon.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_gradient.c
SRCS += $(LVGL_DIR)/src/draw/sw/lv_draw_sw_dither.c

#font
SRCS += $(LVGL_DIR)/src/font/lv_font.c
SRCS += $(LVGL_DIR)/src/font/lv_font_fmt_txt.c
SRCS += $(LVGL_DIR)/src/font/lv_font_loader.c
SRCS += $(LVGL_DIR)/src/font/lv_font_dejavu_16_persian_hebrew.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_8.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_10.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_12.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_12_subpx.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_14.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_16.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_18.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_20.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_22.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_24.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_26.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_28.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_28_compressed.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_30.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_32.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_34.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_36.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_38.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_40.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_42.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_44.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_46.c
SRCS += $(LVGL_DIR)/src/font/lv_font_montserrat_48.c
SRCS += $(LVGL_DIR)/src/font/lv_font_simsun_16_cjk.c
SRCS += $(LVGL_DIR)/src/font/lv_font_unscii_8.c
SRCS += $(LVGL_DIR)/src/font/lv_font_unscii_16.c

#hal
SRCS += $(LVGL_DIR)/src/hal/lv_hal_disp.c
SRCS += $(LVGL_DIR)/src/hal/lv_hal_indev.c
SRCS += $(LVGL_DIR)/src/hal/lv_hal_tick.c

#misc
SRCS += $(LVGL_DIR)/src/misc/lv_anim.c
SRCS += $(LVGL_DIR)/src/misc/lv_anim_timeline.c
SRCS += $(LVGL_DIR)/src/misc/lv_area.c
SRCS += $(LVGL_DIR)/src/misc/lv_async.c
SRCS += $(LVGL_DIR)/src/misc/lv_bidi.c
SRCS += $(LVGL_DIR)/src/misc/lv_color.c
SRCS += $(LVGL_DIR)/src/misc/lv_fs.c
SRCS += $(LVGL_DIR)/src/misc/lv_gc.c
SRCS += $(LVGL_DIR)/src/misc/lv_ll.c
SRCS += $(LVGL_DIR)/src/misc/lv_log.c
SRCS += $(LVGL_DIR)/src/misc/lv_lru.c
SRCS += $(LVGL_DIR)/src/misc/lv_math.c
SRCS += $(LVGL_DIR)/src/misc/lv_mem.c
SRCS += $(LVGL_DIR)/src/misc/lv_printf.c
SRCS += $(LVGL_DIR)/src/misc/lv_style.c
SRCS += $(LVGL_DIR)/src/misc/lv_style_gen.c
SRCS += $(LVGL_DIR)/src/misc/lv_timer.c
SRCS += $(LVGL_DIR)/src/misc/lv_tlsf.c
SRCS += $(LVGL_DIR)/src/misc/lv_txt.c
SRCS += $(LVGL_DIR)/src/misc/lv_txt_ap.c
SRCS += $(LVGL_DIR)/src/misc/lv_utils.c

#widgets
SRCS += $(LVGL_DIR)/src/widgets/lv_arc.c
SRCS += $(LVGL_DIR)/src/widgets/lv_bar.c
SRCS += $(LVGL_DIR)/src/widgets/lv_btn.c
SRCS += $(LVGL_DIR)/src/widgets/lv_btnmatrix.c
SRCS += $(LVGL_DIR)/src/widgets/lv_canvas.c
SRCS += $(LVGL_DIR)/src/widgets/lv_checkbox.c
SRCS += $(LVGL_DIR)/src/widgets/lv_dropdown.c
SRCS += $(LVGL_DIR)/src/widgets/lv_img.c
SRCS += $(LVGL_DIR)/src/widgets/lv_label.c
SRCS += $(LVGL_DIR)/src/widgets/lv_line.c
SRCS += $(LVGL_DIR)/src/widgets/lv_roller.c
SRCS += $(LVGL_DIR)/src/widgets/lv_slider.c
SRCS += $(LVGL_DIR)/src/widgets/lv_switch.c
SRCS += $(LVGL_DIR)/src/widgets/lv_table.c
SRCS += $(LVGL_DIR)/src/widgets/lv_textarea.c

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
