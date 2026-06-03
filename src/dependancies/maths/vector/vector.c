/**
 * @file vector.c
 * @brief Vector utils
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see vector.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "vector.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include "utils/maths/distance/distance.h"
#include <math.h>
#include <stddef.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

float dot_product(const vector_2d_float_t *v, const vector_2d_float_t *u)
{
    SYSTEM_ASSERT(v != NULL);
    SYSTEM_ASSERT(u != NULL);
    return v->x * u->x + v->y * u->y;
}

void cartesian_to_polar(const vector_2d_distance_t *cartesian, vector_2d_polar_t *polar)
{
    SYSTEM_ASSERT(cartesian != NULL);
    SYSTEM_ASSERT(polar != NULL);
    polar->magnitude = sqrtf(cartesian->x * cartesian->x + cartesian->y * cartesian->y);
    polar->angle = atan2f((float)cartesian->y, (float)cartesian->x);
}

void polar_to_cartesian(const vector_2d_polar_t *polar, vector_2d_distance_t *cartesian)
{
    SYSTEM_ASSERT(polar != NULL);
    SYSTEM_ASSERT(cartesian != NULL);
    SYSTEM_ASSERT(polar->magnitude >= 0.0f);
    cartesian->x = (distance_t)(polar->magnitude * cosf(polar->angle));
    cartesian->y = (distance_t)(polar->magnitude * sinf(polar->angle));
}

distance_t vector_2d_distance_length(const vector_2d_distance_t *v)
{
    SYSTEM_ASSERT(v != NULL);
    return pythagore(v->x, v->y);
}

float vector_2d_distance_square(const vector_2d_distance_t *v, const vector_2d_distance_t *u)
{
    SYSTEM_ASSERT(v != NULL);
    SYSTEM_ASSERT(u != NULL);
    distance_t dx = v->x - u->x;
    distance_t dy = v->y - u->y;
    return SQUARE(dx) + SQUARE(dy);
}

distance_t vector_2d_distance(const vector_2d_distance_t *v, const vector_2d_distance_t *u)
{
    SYSTEM_ASSERT(v != NULL);
    SYSTEM_ASSERT(u != NULL);
    return (distance_t)sqrtf(vector_2d_distance_square(v, u));
}

/* ***************************************** Public callback functions definitions *************************************** */
