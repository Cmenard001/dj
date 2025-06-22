/**
 * @file dj_test_params.h
 * @brief Parameters of a dj test
 * @author Cyprien Ménard
 * @date 20/06/2025
 * @see dj_test_params.c
 */

#ifndef __DJ_TEST_PARAMS_H__
#define __DJ_TEST_PARAMS_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj/dj_dependencies/QS_maths/QS_maths.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Parameters of a dj test
 */
typedef struct
{
    GEOMETRY_point_t m_start_point;       /**< Start point of the test */
    GEOMETRY_point_t m_destination_point; /**< Destination point of the test */
} dj_test_params_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize the test parameters with values.
 * @param params Pointer to the test parameters structure to initialize.
 * @param start_point The starting point of the test.
 * @param destination_point The destination point of the test.
 */
void dj_test_params_init(dj_test_params_t *params, GEOMETRY_point_t start_point, GEOMETRY_point_t destination_point);

/**
 * @brief Start a dj test with the given parameters.
 * @param params Pointer to the test parameters structure.
 * @return true on success, false on failure.
 */
bool dj_test_params_start_test(const dj_test_params_t *params);

/* ******************************************* Public callback functions declarations ************************************ */

#endif /* __DJ_TEST_PARAMS_H__ */