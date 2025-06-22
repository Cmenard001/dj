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
    dj_test_params_t m_params; /**< Parameters of the DJ test */
    time_ms_t m_start_time;    /**< Start time of the test in milliseconds */
    time_ms_t m_end_time;      /**< End time of the test in milliseconds */
    bool m_success;            /**< Indicates if the test was successful */
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

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TEST_H__ */
