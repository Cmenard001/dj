/**
 * @file angle.h
 * @brief Angle utils
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 11/10/2025
 * @see angle.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include "utils/maths/distance/distance.h"
#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Constante PI en float
 */
#undef PI
#define PI (3.14159265358979323846f)

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type représentant un angle radian (float) entre -PI et PI
 */
typedef float angle_t;

/**
 * @brief Type représentant un angle en degré (float)
 */
typedef float angle_deg_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction permettant de savoir si deux angles sont égaux
 * @param a1, a2 Les deux angles
 * @param epsilon La tolérance d'égalité
 * @return true Si les angles sont égaux
 * @return false Si les angles ne sont pas égaux
 */
bool angles_equal_with_epsilon(angle_t a1, angle_t a2, angle_t epsilon);

/**
 * @brief Fonction permettant de savoir si deux angles sont égaux
 * @param a1, a2 Les deux angles
 * @param epsilon La tolérance d'égalité
 * @return true Si les angles sont égaux
 * @return false Si les angles ne sont pas égaux
 * @see ANGLE_EQUALITY_EPSILON dans distance.c
 */
bool angles_equal(angle_t a1, angle_t a2);

/**
 * @brief Fonction de conversion d'un angle en rad en degré
 * @param angle L'angle en radians
 * @return angle_deg_t L'angle en degrés
 */
angle_deg_t rad2deg(angle_t angle);

/**
 * @brief Fonction de modulo pour les angles
 * @note Renvoi un angle entre -PI et PI
 * @param angle Angle à moduler
 * @return angle_t Angle modulé
 */
angle_t modulo_angle(angle_t angle);

/* ******************************************* Public callback functions declarations ************************************ */
