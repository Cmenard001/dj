/**
 * @file linear_acceleration.h
 * @brief Management of linear accelerations
 * @author Cyprien Ménard
 * @date 15/10/2025
 * @see linear_acceleration.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/maths/vector/vector.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type for linear acceleration
 * @note in mm/s²
 */
typedef float linear_acceleration_t;

/**
 * @brief Type for linear acceleration 2d vector
 * @note in mm/s
 */
DEFINE_2D_VECTOR_TYPE(linear_acceleration_t, linear_acceleration_2d_vector_t)

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
