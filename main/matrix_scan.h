/**
 * matrix_scan.h
 *  keyboard matrix scanning interface 
 */

#include "matrix.h"

void matrix_init_custom(void);
bool matrix_scan_custom(matrix_row_t* raw);
void matrix_prepare_sleep(void);