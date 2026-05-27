/**
 * @file time.h
 * @brief Time management
 * @author Cyprien Ménard
 * @date 13/10/2025
 * @see time.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Time in milliseconds
 * @warning This type should not overflow (up to ~49 days)
 */
typedef int32_t time_ms_t;

/**
 * @brief Time in microseconds
 * @warning This type can overflow quickly (after ~71 minutes)
 */
typedef int64_t time_us_t;

/**
 * @brief Match time in milliseconds
 */
typedef time_ms_t match_time_ms_t;

/**
 * @brief Timestamp in milliseconds since epoch
 * @warning This type should not overflow (up to year 2262)
 */
typedef uint64_t timestamp_ms_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize the time management module
 */
void time_init(void);

/**
 * @brief Get the absolute time in milliseconds since the program started
 * @return Time in milliseconds
 */
time_ms_t time_get_absolute_time_ms(void);

/**
 * @brief Get the absolute time in microseconds since the program started
 * @return Time in microseconds
 */
time_us_t time_get_absolute_time_us(void);

/* ******************************************* Public callback functions declarations ************************************ */
