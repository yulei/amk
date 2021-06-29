/*******************************************************************************
 * Size: 24 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#include "font.h"

#ifndef FONT_COLLEGE
#define FONT_COLLEGE 1
#endif

#if FONT_COLLEGE

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+002D "-" */
    0xff, 0xff, 0xc0,

    /* U+0030 "0" */
    0x7f, 0x7f, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf,
    0x7, 0xff, 0x7f, 0x0,

    /* U+0031 "1" */
    0x18, 0x71, 0xe3, 0xc1, 0x83, 0x6, 0xc, 0x18,
    0x30, 0x60, 0xc1, 0x83, 0x6, 0x7f, 0xfe,

    /* U+0032 "2" */
    0x7f, 0x7f, 0xf0, 0x60, 0x30, 0x18, 0xc, 0x6,
    0xff, 0xff, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3,
    0x7, 0xff, 0xff, 0x80,

    /* U+0033 "3" */
    0x7f, 0x7f, 0xf0, 0x60, 0x30, 0x18, 0xc, 0x6,
    0x1f, 0xf, 0x80, 0xc0, 0x60, 0x30, 0x18, 0xf,
    0x7, 0xff, 0x7f, 0x0,

    /* U+0034 "4" */
    0x3, 0x1, 0xc0, 0xf0, 0x3c, 0x1f, 0x7, 0xc3,
    0xb1, 0xec, 0x73, 0x38, 0xcf, 0xff, 0xff, 0x3,
    0x0, 0xc0, 0x30, 0x1f, 0x7, 0xc0,

    /* U+0035 "5" */
    0xff, 0xff, 0xf0, 0x18, 0xc, 0x6, 0x3, 0x1,
    0xfe, 0x7f, 0x80, 0xc0, 0x60, 0x30, 0x18, 0xf,
    0x7, 0xff, 0x7f, 0x0,

    /* U+0036 "6" */
    0x7f, 0x7f, 0xf0, 0x78, 0xc, 0x6, 0x3, 0x1,
    0xfe, 0xff, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf,
    0x7, 0xff, 0x7f, 0x0,

    /* U+0037 "7" */
    0xff, 0xff, 0xfc, 0x38, 0xe, 0x7, 0x81, 0xc0,
    0xf0, 0x38, 0xe, 0x7, 0x1, 0xc0, 0x70, 0x38,
    0xe, 0x3, 0x3, 0xf0, 0xfc, 0x0,

    /* U+0038 "8" */
    0x7f, 0x7f, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0xff, 0xff, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf,
    0x7, 0xff, 0x7f, 0x0,

    /* U+0039 "9" */
    0x7f, 0x7f, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0xff, 0x7f, 0x80, 0xc0, 0x60, 0x30, 0x18, 0xf,
    0x7, 0xff, 0x7f, 0x0,

    /* U+003A ":" */
    0xf0, 0x0, 0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 188, .box_w = 9, .box_h = 2, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 3, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 188, .box_w = 7, .box_h = 17, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 188, .box_w = 10, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 188, .box_w = 10, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 162, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 188, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 75, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint8_t glyph_id_ofs_list_0[] = {
    0, 0, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 45, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = glyph_id_ofs_list_0, .list_length = 14, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
};


#if 0
/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t FONT_COLLEGE = {
#else
lv_font_t FONT_COLLEGE = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};

#endif


#endif /*#if FONT_COLLEGE*/

