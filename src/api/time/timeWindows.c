/**
 * @file timeWindows.c
 * @brief Time management API implementation for Windows
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see timeWindows.c
 */

#ifndef __TIMEWINDOWS_H__
#define __TIMEWINDOWS_H__

/* ******************************************************* Includes ****************************************************** */
#include "time.h"

#include <windows.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void time_init()
{
    // Initialize time
}

time_us_t time_us()
{
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (time_us_t)((counter.QuadPart * 1000000) / frequency.QuadPart);
}

time_ms_t time_ms()
{
    return GetTickCount();
}

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __TIMEWINDOWS_H__ */