/**
 * @file dj_tester.h
 * @brief Tester for the DJ module
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_tester.c
 */

#ifndef __DJ_TESTER_H__
#define __DJ_TESTER_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj_test_set/dj_test_set.h"
#include "dj_test_suite/dj_test_suite.h"

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Launches the DJ module tests.
 *
 * This function initializes the test set and runs all the tests defined in the DJ module.
 */
void dj_tester_launch_tests();

/**
 * @brief Get the test suites configured for the benchmark.
 *
 * Exposes the internal suite table so that unit tests can run the exact same
 * suites as the benchmark and assert that every test finds a path.
 *
 * @param count Output: number of suites returned.
 * @return Pointer to a read-only array of @p count suite configurations.
 */
const dj_test_suite_config_t *dj_tester_get_suites(uint32_t *count);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TESTER_H__ */