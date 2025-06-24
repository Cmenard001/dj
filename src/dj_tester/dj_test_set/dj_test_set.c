/**
 * @file dj_test_set.c
 * @brief Set of DJ tests
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test_set.h
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_test_set.h"
#include <stdio.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void dj_test_set_init(dj_test_set_t *test_set)
{
    // Initialize the number of tests to zero
    test_set->m_num_tests = 0;
}

bool dj_test_set_add_test(dj_test_set_t *test_set, dj_test_t test)
{
    // Check if there is space in the test set
    if (test_set->m_num_tests < DJ_TEST_SET_MAX_TESTS)
    {
        // Add the test to the set
        test_set->m_tests[test_set->m_num_tests] = test;
        // Increment the number of tests
        test_set->m_num_tests++;
        return true; // Test added successfully
    }
    return false; // Test set is full
}

void dj_test_set_run_tests(dj_test_set_t *test_set)
{
    // Iterate through each test in the set and execute it
    for (uint16_t i = 0; i < test_set->m_num_tests; i++)
    {
        // Start the test
        dj_test_start(&test_set->m_tests[i]);
    }
}

time_ms_t dj_test_set_get_duration(dj_test_set_t *test_set)
{
    // Calculate the total duration of all tests in the set
    time_ms_t total_duration = 0;
    for (uint16_t i = 0; i < test_set->m_num_tests; i++)
    {
        total_duration += dj_test_get_duration(&test_set->m_tests[i]);
    }
    return total_duration;
}

uint32_t dj_test_set_get_success_count(dj_test_set_t *test_set)
{
    // Count the number of successful tests in the set
    uint32_t success_count = 0;
    for (uint16_t i = 0; i < test_set->m_num_tests; i++)
    {
        if (test_set->m_tests[i].m_success)
        {
            success_count++;
        }
    }
    return success_count;
}

void dj_test_get_report(dj_test_set_t *test_set, dj_test_report_t *report)
{
    // Initialize the report
    dj_test_report_init(report);

    char l_result[DJ_TEST_REPORT_MAX_LENGTH];
    snprintf(l_result,
             DJ_TEST_REPORT_MAX_LENGTH,
             "Test Set Report: %u tests executed\nDone in %llu ms. Success rate : %u/%u\n",
             (unsigned int)test_set->m_num_tests,
             (unsigned long long)dj_test_set_get_duration(test_set),
             (unsigned int)dj_test_set_get_success_count(test_set),
             (unsigned int)test_set->m_num_tests);
    dj_test_report_add_message(report, l_result);
}

/* ***************************************** Public callback functions definitions *************************************** */
