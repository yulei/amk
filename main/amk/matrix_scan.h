/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file matrix_scan.h
 * @brief keyboard matrix scanning interface 
 */

#include "matrix.h"

void matrix_init_custom(void);
bool matrix_scan_custom(matrix_row_t* raw);
void matrix_prepare_sleep(void);