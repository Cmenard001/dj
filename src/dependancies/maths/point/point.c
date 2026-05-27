/**
 * @file point.c
 * @brief Point utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see point.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "point.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include "utils/maths/angle/angle.h"
#include "utils/maths/distance/distance.h"
#include <math.h>
#include <stddef.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Epsilon par défaut pour la comparaison de distances
 * @note Utile pour compenser les erreurs d'arrondi des floats
 */
#define DISTANCE_COMPARISON_EPSILON 0.0001f

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

bool point_compare_with_epsilon(const point_t *a, const point_t *b, distance_t epsilon)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    SYSTEM_ASSERT(epsilon >= 0.0f);
    return point_distance_square(a, b) <= SQUARE(epsilon);
}

bool point_compare(const point_t *a, const point_t *b)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    return point_compare_with_epsilon(a, b, DISTANCE_COMPARISON_EPSILON);
}

distance_t point_distance(const point_t *a, const point_t *b)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    return (distance_t)sqrtf(point_distance_square(a, b));
}

float point_distance_square(const point_t *a, const point_t *b)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    distance_t dx = a->x - b->x;
    distance_t dy = a->y - b->y;
    return (dx * dx) + (dy * dy);
}

distance_t point_manhattan_distance(const point_t *a, const point_t *b)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    distance_t dx = a->x - b->x;
    distance_t dy = a->y - b->y;
    return fabsf(dx) + fabsf(dy);
}

float point_cross_product(const point_t *a, const point_t *b, const point_t *c)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    SYSTEM_ASSERT(c != NULL);
    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
}

bool vector_resize_absolute(const point_t *end,
                            const point_t *start,
                            point_t *new_end,
                            distance_t norme)
{
    SYSTEM_ASSERT(end != NULL);
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(new_end != NULL);
    SYSTEM_ASSERT(norme >= 0.0f);
    distance_t delta_y = end->y - start->y;
    distance_t delta_x = end->x - start->x;
    if ((distance_null(delta_x) && distance_null(delta_y)) || (norme < 0))
    {
        return false;
    }
    float angle = atan2f(delta_y, delta_x);
    new_end->x = start->x + cosf(angle) * norme;
    new_end->y = start->y + sinf(angle) * norme;
    return true;
}

bool vector_resize_relative(const point_t *end,
                            const point_t *start,
                            point_t *new_end,
                            distance_t offset)
{
    SYSTEM_ASSERT(end != NULL);
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(new_end != NULL);
    SYSTEM_ASSERT(offset >= 0.0f);
    distance_t delta_y = end->y - start->y;
    distance_t delta_x = end->x - start->x;
    if ((distance_null(delta_x) && distance_null(delta_y)) || (offset < 0))
    {
        return false;
    }
    angle_t angle = atan2f(delta_y, delta_x);
    distance_t norme = pythagore(delta_x, delta_y) + offset;
    new_end->x = start->x + cosf(angle) * norme;
    new_end->y = start->y + sinf(angle) * norme;
    return true;
}

void point_translate(point_t *point, const vector_2d_distance_t *vector)
{
    point->x += vector->x;
    point->y += vector->y;
}

angle_t point_compute_angle_to(const point_t *source, const point_t *target)
{
    SYSTEM_ASSERT(source != NULL);
    SYSTEM_ASSERT(target != NULL);
    return atan2f(target->y - source->y, target->x - source->x);
}

/* ***************************************** Public callback functions definitions *************************************** */
