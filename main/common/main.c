/**
 * main.c
 *  main loop
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