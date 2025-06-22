/**
 * @file timeLinux.c
 * @brief Time management API implementation for Linux
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see timeLinux.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "time.h"

#include <sys/time.h>
#include <unistd.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void time_init()
{
    // Initialize time
}

time_us_t time_us()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (time_us_t)(tv.tv_sec * 1000000 + tv.tv_usec);
}

time_ms_t time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (time_ms_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/* ***************************************** Public callback functions definitions *************************************** */