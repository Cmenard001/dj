/**
 * @file dj_test_set.h
 * @brief Set of DJ tests
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test_set.c
 */

#ifndef __DJ_TEST_SET_H__
#define __DJ_TEST_SET_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj_tester/dj_test/dj_test.h"
#include "dj_tester/dj_test_report/dj_test_report.h"

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of tests in a test set
 *
 * This macro defines the maximum number of tests that can be included in a test set.
 * It is used to allocate memory and manage the test set.
 */
#define DJ_TEST_SET_MAX_TESTS (200)

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure representing a set of DJ tests
 *
 * This structure holds an array of tests and the number of tests in the set.
 */
typedef struct
{
    dj_test_t m_tests[DJ_TEST_SET_MAX_TESTS]; /**< Array of tests in the set */
    uint16_t m_num_tests;                     /**< Number of tests in the set */
} dj_test_set_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize a DJ test set
 *
 * This function initializes a DJ test set by setting the number of tests to zero.
 *
 * @param test_set Pointer to the test set to initialize
 */
void dj_test_set_init(dj_test_set_t *test_set);

/**
 * @brief Add a test to a DJ test set
 *
 * This function adds a test to the specified DJ test set. It checks if there is space
 * in the set before adding the test.
 *
 * @param test_set Pointer to the test set
 * @param test The test to add
 * @return true if the test was added successfully, false if the set is full
 */
bool dj_test_set_add_test(dj_test_set_t *test_set, dj_test_t test);

/**
 * @brief Run all tests in a DJ test set
 *
 * This function runs all tests in the specified DJ test set. It iterates through each
 * test and executes it, collecting results.
 *
 * @param test_set Pointer to the test set to run
 */
void dj_test_set_run_tests(dj_test_set_t *test_set);

/**
 * @brief Get the duration of a DJ test set
 *
 * This function calculates the total duration of all tests in the specified DJ test set.
 * It sums the durations of each individual test.
 *
 * @param test_set Pointer to the test set
 * @return Total duration of the test set in milliseconds
 */
time_ms_t dj_test_set_get_duration(dj_test_set_t *test_set);

/**
 * @brief Get the number of successful tests in a DJ test set
 *
 * This function counts the number of tests that were successful in the specified DJ test set.
 *
 * @param test_set Pointer to the test set
 * @return Number of successful tests
 */
uint32_t dj_test_set_get_success_count(dj_test_set_t *test_set);

/**
 * @brief Get the number of tests in a DJ test set
 *
 * @param test_set Pointer to the test set
 * @param report Pointer to the test report to fill with the results
 */
void dj_test_get_report(dj_test_set_t *test_set, dj_test_report_t *report);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TEST_SET_H__ */