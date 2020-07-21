/**
 * rtt.h
 */
#pragma once

#ifdef SEGGER_RTT_ENABLE
    #include "SEGGER_RTT.h"
    #define rtt_printf(x, fmt, ...) SEGGER_RTT_printf(x, fmt, ##__VA_ARGS__)
#else
    #define rtt_printf(x, fmt, ...)
#endif
