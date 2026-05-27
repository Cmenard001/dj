/**
 * @file angular_speed.h
 * @brief Management of angular speeds
 * @author Cyprien Ménard
 * @date 15/10/2025
 * @see angular_speed.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */
#include "utils/maths/vector/vector.h"

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type for angular speed
 * @note in rad/s
 */
typedef float angular_speed_t;

/**
 * @brief Type for angular speed 2d vector
 * @note in rad/s
 */
DEFINE_2D_VECTOR_TYPE(angular_speed_t, angular_speed_2d_vector_t)

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
