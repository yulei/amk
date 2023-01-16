
#include "matrix.h"
#include "action.h"
#include "action_util.h"
#include "mousekey.h"
#include "host.h"
#include "suspend.h"

void suspend_idle(uint8_t time)
{
}

void suspend_power_down(void)
{
}

__attribute__ ((weak)) void matrix_power_up(void) {}
__attribute__ ((weak)) void matrix_power_down(void) {}
__attribute__ ((weak))
bool suspend_wakeup_condition(void)
{
    matrix_power_up();
    matrix_scan();
    matrix_power_down();
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (matrix_get_row(r)) return true;
    }
    return false;
}

// run immediately after wakeup
void suspend_wakeup_init(void)
{
    // clear keyboard state
    // need to do it manually, because we're running from ISR
    //  and clear_keyboard() calls print
    // so only clear the variables in memory
    // the reports will be sent from main.c afterwards
    // or if the PC asks for GET_REPORT
    clear_mods();
    clear_weak_mods();
    clear_keys();
#ifdef HID_OTHER_ENABLE 
    mousekey_clear();
    host_system_send(0);
    host_consumer_send(0);
#endif
}
