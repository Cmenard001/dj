/**
 * @file position.c
 * @brief Position utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see position.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "position.h"
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

void viewing_algebric_distance(const position_t *start,
                               const point_t *destination,
                               distance_t *distance)
{
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(destination != NULL);
    SYSTEM_ASSERT(distance != NULL);
    distance_t deltaX, deltaY;
    deltaX = (distance_t)absolute(destination->x - start->point.x);
    deltaY = (distance_t)absolute(destination->y - start->point.y);

    if (absolute(start->angle) > PI / 2)
    {
        *distance = -pythagore(deltaX, deltaY);
    }
    else
    {
        *distance = pythagore(deltaX, deltaY);
    }
    // en mm
}

void calculate_point_from_distance_and_position(const position_t *start_pos,
                                                distance_t dist,
                                                point_t *point)
{
    SYSTEM_ASSERT(start_pos != NULL);
    SYSTEM_ASSERT(point != NULL);
    point->x = start_pos->point.x + dist * cosf(start_pos->angle);
    point->y = start_pos->point.y + dist * sinf(start_pos->angle);
}

void viewing_angle(const point_t *start, const point_t *destination, angle_t *angle)
{
    SYSTEM_ASSERT(start != NULL);
    SYSTEM_ASSERT(destination != NULL);
    SYSTEM_ASSERT(angle != NULL);
    *angle = (angle_t)(atan2f(destination->y - start->y, destination->x - start->x));
}

void translate_position(const position_t *pos,
                        const vector_2d_distance_t *translation,
                        point_t *translated_point)
{
    SYSTEM_ASSERT(pos != NULL);
    SYSTEM_ASSERT(translation != NULL);
    SYSTEM_ASSERT(translated_point != NULL);
    translated_point->x =
        pos->point.x + translation->x * cosf(pos->angle) - translation->y * sinf(pos->angle);
    translated_point->y =
        pos->point.y + translation->x * sinf(pos->angle) + translation->y * cosf(pos->angle);
}

bool project_position_on_segment(const position_t *pos,
                                 const segment_t *seg,
                                 point_t *projected_pos)
{
    SYSTEM_ASSERT(pos != NULL);
    SYSTEM_ASSERT(seg != NULL);
    SYSTEM_ASSERT(projected_pos != NULL);

    distance_t dx = seg->b.x - seg->a.x;
    distance_t dy = seg->b.y - seg->a.y;
    distance_t c = cosf(pos->angle);
    distance_t s = sinf(pos->angle);

    float denom = c * dy - s * dx;

    /* Check if parallel */
    if (absolute(denom) < 0.0001f)
    {
        return false;
    }

    float t1 = ((seg->a.x - pos->point.x) * dy - (seg->a.y - pos->point.y) * dx) / denom;
    float t2 = ((seg->a.x - pos->point.x) * s - (seg->a.y - pos->point.y) * c) / denom;

    if (t1 >= 0.0f && t2 >= 0.0f && t2 <= 1.0f)
    {
        projected_pos->x = pos->point.x + t1 * c;
        projected_pos->y = pos->point.y + t1 * s;
        return true;
    }

    return false;
}

/* ***************************************** Public callback functions definitions *************************************** */
