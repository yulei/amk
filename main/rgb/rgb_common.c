/**
 * rgb_common.c
 *  rgb common implementation
 */

#include "rgb_common.h"

static const uint8_t CIE1931_CURVE[256] = {
    0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,
    4,   4,   5,   5,   5,   5,   5,   6,   6,   6,   6,   6,   7,   7,   7,   7,
    7,   8,   8,   8,   8,   9,   9,   9,   9,  10,  10,  10,  11,  11,  11,  12,
   12,  12,  13,  13,  13,  14,  14,  14,  15,  15,  15,  16,  16,  17,  17,  17,
   18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  23,  24,  24,  25,  25,
   26,  26,  27,  27,  28,  29,  29,  30,  30,  31,  32,  32,  33,  34,  34,  35,
   36,  36,  37,  38,  38,  39,  40,  41,  41,  42,  43,  44,  45,  45,  46,  47,
   48,  49,  50,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
   63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  76,  77,  78,  79,
   80,  81,  83,  84,  85,  86,  88,  89,  90,  91,  93,  94,  95,  97,  98, 100,
  101, 102, 104, 105, 107, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 123,
  125, 126, 128, 130, 131, 133, 135, 136, 138, 140, 142, 143, 145, 147, 149, 150,
  152, 154, 156, 158, 160, 162, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181,
  183, 186, 188, 190, 192, 194, 196, 198, 201, 203, 205, 207, 209, 212, 214, 216,
  219, 221, 223, 226, 228, 231, 233, 235, 238, 240, 243, 245, 248, 250, 253, 255
};

 // the converter comes from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
rgb_t hsv_to_rgb(hsv_t hsv)
{
    rgb_t rgb;
    uint8_t region, remainder, p, q, t;
    uint16_t h, s, v;
    if (hsv.s == 0)
    {
        rgb.r = rgb.g = rgb.b = CIE1931_CURVE[hsv.v];
        return rgb;
    }
    h = hsv.h;
    s = hsv.s;
    v = CIE1931_CURVE[hsv.v];

    region = h / 43;
    remainder = (h - (region * 43)) * 6; 

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = v;
            break;
        default:
            rgb.r = v; rgb.g = p; rgb.b = q;
            break;
    }
    return rgb;
}

rgb_t pick_color(uint8_t hue)
{
    hsv_t hsv = {hue, 255, 255};
    return hsv_to_rgb(hsv);
}
