/**
 * @file distance.c
 * @brief Distance definitions
 * @author Cyprien Ménard
 * @date 12/10/2025
 * @see distance.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "distance.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include <math.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Tolérance d'égalité entre deux distances
 */
#define DISTANCE_EQUALITY_EPSILON 0.01f

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

distance_t pythagore(distance_t d1, distance_t d2)
{
    return sqrtf(SQUARE(d1) + SQUARE(d2));
}

bool distances_equal_with_epsilon(distance_t d1, distance_t d2, distance_t epsilon)
{
    SYSTEM_ASSERT(epsilon >= 0.0f);
    return fabsf(d1 - d2) < epsilon;
}

bool distances_equal(distance_t d1, distance_t d2)
{
    return distances_equal_with_epsilon(d1, d2, DISTANCE_EQUALITY_EPSILON);
}

bool distance_null(distance_t d)
{
    return fabsf(d) < DISTANCE_EQUALITY_EPSILON;
}

/* ***************************************** Public callback functions definitions *************************************** */
