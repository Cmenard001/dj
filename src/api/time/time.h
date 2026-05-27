/**
 * @file time.h
 * @brief Time management API
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see time.c
 */

#ifndef __TIME_H__
#define __TIME_H__

/* ******************************************************* Includes ****************************************************** */

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Time in milliseconds
 */
typedef int64_t time_ms_t;

/**
 * @brief Time in microseconds
 */
typedef int64_t time_us_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize time
 */
void time_init();

/**
 * @brief Get time in microseconds
 *
 * @return time_us_t Time in microseconds
 */
time_us_t time_us();

/**
 * @brief Get time in milliseconds
 *
 * @return time_ms_t Time in milliseconds
 */
time_ms_t time_ms();

/**
 * @brief Get CPU time consumed by this process in microseconds
 *
 * @details On Linux, uses CLOCK_PROCESS_CPUTIME_ID so preempted time is excluded.
 *          On other platforms, falls back to time_us().
 * @return time_us_t CPU time in microseconds
 */
time_us_t time_cpu_us();

/**
 * @brief Get absolute time in milliseconds (same as time_ms)
 */
static inline time_ms_t time_get_absolute_time_ms(void) { return time_ms(); }

/**
 * @brief Get absolute time in microseconds (same as time_us)
 */
static inline time_us_t time_get_absolute_time_us(void) { return time_us(); }

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __TIME_H__ */