/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file main.c
 */

#include "board.h"

/*------------- MAIN -------------*/
int main(void)
{
    board_init();

    while (1) {
        board_task();
    }

    return 0;
}