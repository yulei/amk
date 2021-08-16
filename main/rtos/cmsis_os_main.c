/**
 * @file cmsis_os_main.c
 */

#include "cmsis_os2.h"
#include "board.h"
#include "usb_interface.h"

void usb_thread(void *argument);
void main_thread(void *argument);

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    osKernelInitialize();
    osThreadNew(usb_thread, NULL, NULL);
    osThreadNew(main_thread, NULL, NULL);
    osKernelStart();

    // never reach here
    for (;;);
    return 0;
}

void usb_thread(void *arg)
{
    usb_init();
    while (1) {
        usb_task();
    }
}

void main_thread(void *arg)
{
    while(1) {
        board_task();
    }
}