/**
 * @file linear_speed.h
 * @brief Management of linear speeds
 * @author Cyprien Ménard
 * @date 15/10/2025
 * @see linear_speed.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/maths/vector/vector.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type for linear speed
 * @note in mm/s
 */
typedef float linear_speed_t;

/**
 * @brief Type for linear speed 2d vector
 * @note in mm/s
 */
DEFINE_2D_VECTOR_TYPE(linear_speed_t, linear_speed_2d_vector_t)

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
