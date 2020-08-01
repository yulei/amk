/**
 * rtt.h
 */
#pragma once

#ifndef SEGGER_RTT_ENABLE
#define SEGGER_RTT_ENABLE 1
#endif

#if SEGGER_RTT_ENABLE
    #include "SEGGER_RTT.h"
    #define rtt_printf(x, fmt, ...) SEGGER_RTT_printf(x, fmt, ##__VA_ARGS__)
#else
    #define rtt_printf(...) 1
#endif
