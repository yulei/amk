/**
 * @file usbmaster_matrix.c
 */

#include <stdbool.h>
#include "matrix.h"

static matrix_row_t matrix[MATRIX_ROWS];

void matrix_init(void)
{
}

uint8_t matrix_scan(void)
{
    return 1;
}

matrix_row_t matrix_get_row(uint8_t row) { return matrix[row]; }