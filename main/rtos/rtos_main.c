/**
 * @file rtos_main.c
 */

#include "tx_api.h"
#include "board.h"

#define MAIN_STACK_SIZE         1024
static CHAR main_stack[MAIN_STACK_SIZE];

TX_THREAD main_thread;
void main_thread_entry(ULONG thread_input);

void tx_application_define(void *first_unused_memory)
{
    tx_thread_create(&main_thread, "main thread", main_thread_entry, 0, main_stack, MAIN_STACK_SIZE, 
                    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    // start threadx
    tx_kernel_enter();

    // never reach here
    return 0;
}

void main_thread_entry(ULONG thread_input)
{
    extern void usb_init(void);
    usb_init();
    while (1) {
            board_task();
    }
}