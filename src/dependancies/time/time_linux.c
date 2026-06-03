/**
 * @file time_linux.c
 * @brief Time management
 * @author Cyprien Ménard
 * @date 13/10/2025
 * @see time.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "time.h"
#include <time.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

static struct timespec start_time;

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

inline void time_init(void)
{
    clock_gettime(CLOCK_MONOTONIC, &start_time);
}

inline time_ms_t time_get_absolute_time_ms(void)
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (time_ms_t)(((int64_t)(current_time.tv_sec - start_time.tv_sec)) * 1000 +
                       (current_time.tv_nsec - start_time.tv_nsec) / 1000000);
}

inline time_us_t time_get_absolute_time_us(void)
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (time_us_t)(((int64_t)(current_time.tv_sec - start_time.tv_sec)) * 1000000 +
                       (current_time.tv_nsec - start_time.tv_nsec) / 1000);
}

/* ***************************************** Public callback functions definitions *************************************** */
