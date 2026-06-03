/**
 * @file distance.h
 * @brief Distance definitions
 * @author Cyprien Ménard
 * @date 12/10/2025
 * @see distance.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once

/* ******************************************************* Includes ****************************************************** */

#include <stdbool.h>
#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Type définissant une distance en mm
 */
typedef float distance_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Fonction permettant de récupérer la distance entre deux points
 *        en utilisant le théorème de pythagore
 * @param d1, d2 Les deux distances
 * @return distance_t La distance entre les deux points
 */
distance_t pythagore(distance_t d1, distance_t d2);

/**
 * @brief Fonction permettant de savoir si deux distances sont égales
 * @param d1, d2 Les deux distances
 * @param epsilon La tolérance d'égalité
 * @return true Si les distances sont égales
 * @return false Si les distances ne sont pas égales
 */
bool distances_equal_with_epsilon(distance_t d1, distance_t d2, distance_t epsilon);

/**
 * @brief Fonction permettant de savoir si deux distances sont égales
 * @param d1, d2 Les deux distances
 * @return true Si les distances sont égales
 * @return false Si les distances ne sont pas égales
 * @see DISTANCE_EQUALITY_EPSILON dans distance.c
 */
bool distances_equal(distance_t d1, distance_t d2);

/**
 * @brief Fonction permettant de savoir si une distance est nulle
 * @param d La distance
 * @return true Si la distance est nulle
 * @return false Si la distance n'est pas nulle
 * @see DISTANCE_NULLITY_EPSILON dans distance.c
 */
bool distance_null(distance_t d);

/* ******************************************* Public callback functions declarations ************************************ */
