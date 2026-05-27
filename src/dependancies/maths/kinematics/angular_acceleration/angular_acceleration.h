/**
 * @file angular_acceleration.h
 * @brief Management of angular accelerations
 * @author Cyprien Ménard
 * @date 14/10/2025
 * @see angular_acceleration.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/maths/vector/vector.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type for angular acceleration
 * @note in rad/s²
 */
typedef float angular_acceleration_t;

/**
 * @brief Type for angular acceleration 2d vector
 * @note in rad/s
 */
DEFINE_2D_VECTOR_TYPE(angular_acceleration_t, angular_acceleration_2d_vector_t)

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
