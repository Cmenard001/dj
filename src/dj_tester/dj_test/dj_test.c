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
    test->m_params       = *params;
    test->m_start_time   = time_ms();
    test->m_end_time     = 0;
    test->m_cpu_start_us = 0;
    test->m_cpu_end_us   = 0;
    test->m_success      = false;
}

void dj_test_start(dj_test_t *test)
{
    test->m_start_time   = time_ms();
    test->m_cpu_start_us = time_cpu_us();
    test->m_success      = dj_test_params_start_test(&test->m_params);
    test->m_cpu_end_us   = time_cpu_us();
    test->m_end_time     = time_ms();
}

time_ms_t dj_test_get_duration(const dj_test_t *test)
{
    return test->m_end_time - test->m_start_time;
}

time_us_t dj_test_get_cpu_duration(const dj_test_t *test)
{
    return test->m_cpu_end_us - test->m_cpu_start_us;
}

/* ***************************************** Public callback functions definitions *************************************** */