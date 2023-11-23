/**
 * @file usage_table.c
 * @author astro
 * @brief convert keyboard usage to character
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stddef.h>

#include "keycode.h"
#include "usage_table.h"
#include "amk_utils.h"

#define USAGE_INVALID_INDEX     0xFF

typedef struct {
    uint8_t     code;
    char        upper;
    char        lower;
} usage_item_t;

static const usage_item_t usage_alphas[] = {
    {KC_A, 'A', 'a'},
    {KC_B, 'B', 'b'},
    {KC_C, 'C', 'c'},
    {KC_D, 'D', 'd'},
    {KC_E, 'E', 'e'},
    {KC_F, 'F', 'f'},
    {KC_G, 'G', 'g'},
    {KC_H, 'H', 'h'},
    {KC_I, 'I', 'i'},
    {KC_J, 'J', 'j'},
    {KC_K, 'K', 'k'},
    {KC_L, 'L', 'l'},
    {KC_M, 'M', 'm'},
    {KC_N, 'N', 'n'},
    {KC_O, 'O', 'o'},
    {KC_P, 'P', 'p'},
    {KC_Q, 'Q', 'q'},
    {KC_R, 'R', 'r'},
    {KC_S, 'S', 's'},
    {KC_T, 'T', 't'},
    {KC_U, 'U', 'u'},
    {KC_V, 'V', 'v'},
    {KC_W, 'W', 'w'},
    {KC_X, 'X', 'x'},
    {KC_Y, 'Y', 'y'},
    {KC_Z, 'Z', 'z'},
};

static const usage_item_t usage_puncts[] = {
    {KC_1, '!', '1'},
    {KC_2, '@', '2'},
    {KC_3, '#', '3'},
    {KC_4, '$', '4'},
    {KC_5, '%', '5'},
    {KC_6, '^', '6'},
    {KC_7, '&', '7'},
    {KC_8, '*', '8'},
    {KC_9, '(', '9'},
    {KC_0, ')', '0'},
    {KC_MINUS, '_', '-'},
    {KC_EQUAL, '+', '='},
    {KC_LBRC, '{', '['},
    {KC_RBRC, '}', ']'},
    {KC_BSLS, '|', '\\'},
    {KC_SCLN, ':', ';'},
    {KC_QUOTE, '\"', '\''},
    {KC_GRAVE, '~', '`'},
    {KC_COMMA, '<', ','},
    {KC_DOT, '>', '.'},
    {KC_SLASH, '?', '/'},
};

uint8_t get_usage_index(const usage_item_t *items, size_t size, uint8_t keycode)
{
    for (int i = 0; i < size; i++) {
        if (keycode == items[i].code) {
            return i;
        }
    }
    return USAGE_INVALID_INDEX;
}

char usage2char(uint8_t keycode, bool shift, bool caps)
{
    uint8_t index = get_usage_index(usage_alphas, AMK_ARRAY_SIZE(usage_alphas), keycode);
    if (index != USAGE_INVALID_INDEX) {
        if (caps) {
            if (shift) {
                return usage_alphas[index].lower;
            } else {
                return usage_alphas[index].upper;
            }
        } else {
            if (shift) {
                return usage_alphas[index].upper;
            } else {
                return usage_alphas[index].lower;
            }
        }
    }

    index = get_usage_index(usage_puncts, AMK_ARRAY_SIZE(usage_puncts), keycode);
    if (index != USAGE_INVALID_INDEX) {
        if (shift) {
            return usage_puncts[index].upper;
        } else {
            return usage_puncts[index].lower;
        }
    }

    return '\0';
}
