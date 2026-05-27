/**
 * @file dj_test.h
 * @brief Representation of a DJ test
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test.c
 */

#ifndef __DJ_TEST_H__
#define __DJ_TEST_H__

/* ******************************************************* Includes ****************************************************** */

#include "api/time/time.h"
#include "dj_tester/dj_test_params/dj_test_params.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a DJ test.
 */
typedef struct
{
    dj_test_params_t m_params;        /**< Parameters of the DJ test */
    time_ms_t        m_start_time;    /**< Wall-clock start time [ms] */
    time_ms_t        m_end_time;      /**< Wall-clock end time [ms] */
    time_us_t        m_cpu_start_us;  /**< CPU start time [µs] */
    time_us_t        m_cpu_end_us;    /**< CPU end time [µs] */
    bool             m_success;       /**< Indicates if the test was successful */
} dj_test_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a DJ test with given parameters.
 * @param test Pointer to the DJ test structure to initialize.
 * @param params Parameters of the DJ test.
 */
void dj_test_init(dj_test_t *test, dj_test_params_t *params);

/**
 * @brief Start the DJ test.
 * @param test Pointer to the DJ test structure to start.
 */
void dj_test_start(dj_test_t *test);

/**
 * @brief Get the duration of the DJ test in milliseconds.
 * @param test Pointer to the DJ test structure.
 * @return Duration of the test in milliseconds.
 */
time_ms_t dj_test_get_duration(const dj_test_t *test);

/**
 * @brief Get the CPU time consumed by the DJ test in microseconds.
 * @param test Pointer to the DJ test structure.
 * @return CPU time of the test in microseconds.
 */
time_us_t dj_test_get_cpu_duration(const dj_test_t *test);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TEST_H__ */
