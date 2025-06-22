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

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TESTER_H__ */