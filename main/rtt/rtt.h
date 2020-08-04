/**
 * rtt.h
 */
#pragma once

#ifndef SEGGER_RTT_ENABLE
#define SEGGER_RTT_ENABLE 1
#endif

#if SEGGER_RTT_ENABLE
    #include "SEGGER_RTT.h"
    #define rtt_printf(fmt, ...) SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#else
    #define rtt_printf(...)
#endif
