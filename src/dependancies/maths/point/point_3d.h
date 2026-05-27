/**
 * @file point_3d.h
 * @brief Point 3D utilities
 * @author Dorian Benech
 * @date 18/02/2026
 * @see point_3d.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/distance/distance.h"
#include "utils/maths/vector/vector.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type définissant un point en 3D
 * @note Hérite de vector_3d_distance_t car un point est un vecteur avec une origine en (0,0,0)
 */
typedef vector_3d_distance_t point_3d_t;

/* *********************************************** Public functions declarations ***************************************** */

// TODO: all 3d point operations (distance, comparison, translation, etc.)

/* ******************************************* Public callback functions declarations ************************************ */
