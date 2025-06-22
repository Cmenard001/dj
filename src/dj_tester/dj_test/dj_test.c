/**
 * @file dj_test.c
 * @brief Representation of a DJ test
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_test.h"

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_test_init(dj_test_t *test, dj_test_params_t *params)
{
    // Initialize the test parameters
    test->m_params = *params;
    // Set the start time to the current time in milliseconds
    test->m_start_time = time_ms();
    // Set the end time to 0 (not finished yet)
    test->m_end_time = 0;
    // Set success to false (not successful yet)
    test->m_success = false;
}

void dj_test_start(dj_test_t *test)
{
    // Set the start time to the current time in milliseconds
    test->m_start_time = time_ms();
    // Start the test
    test->m_success = dj_test_params_start_test(&test->m_params);
    // Set the end time to the current time in milliseconds
    test->m_end_time = time_ms();
}

time_ms_t dj_test_get_duration(const dj_test_t *test)
{
    return test->m_end_time - test->m_start_time;
}

/* ***************************************** Public callback functions definitions *************************************** */