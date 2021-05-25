/**
 * rf_power.c
 */

#include <stddef.h>
#include "rf_power.h"

typedef struct {
    rf_peripheral_sleep_t sleep;       ///< sleep prepare callback
    void* context;                  ///< registered context
} rf_sleep_t;

typedef struct {
    rf_sleep_t  modules[RF_POWER_MODULE_MAX];   ///< registered modules
    uint32_t current;                           ///< current module
} rf_power_t;

static rf_power_t rf_power;

void rf_power_init(void)
{
    for( int i = 0; i < RF_POWER_MODULE_MAX; i++) {
        rf_power.modules[i].sleep = NULL;
        rf_power.modules[i].context = NULL;
    }

    rf_power.current = 0;
}

uint32_t rf_power_register(rf_peripheral_sleep_t sleep, void* context)
{
    if (rf_power.current < RF_POWER_MODULE_MAX) {
        rf_power.modules[rf_power.current].sleep = sleep;
        rf_power.modules[rf_power.current].context = context;
        rf_power.current++;
        return 0;
    } 

    return 1;
}

void rf_power_prepare_sleep(void)
{
    for (uint32_t i = 0; i < rf_power.current; i++) {
        rf_power.modules[i].sleep(rf_power.modules[i].context);
    }
}

void rf_power_uninit(void)
{
    rf_power.current = 0;
}
