/**
 * @file circle.c
 * @brief Circles utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see circle.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "circle.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include "utils/maths/distance/distance.h"
#include "utils/maths/point/point.h"
#include "utils/maths/segment/segment.h"
#include <math.h>
#include <stddef.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* ********************************************** Public functions definitions ******************************************* */

void circle_from_diameter(const segment_t *diameter, circle_t *out)
{
    SYSTEM_ASSERT(diameter != NULL);
    SYSTEM_ASSERT(out != NULL);
    point_t middle;
    segment_middle(diameter, &middle);
    out->c = middle;
    out->r = point_distance(&out->c, &diameter->a);
}

// https://stackoverflow.com/questions/4103405/what-is-the-algorithm-for-finding-the-center-of-a-circle-from-three-points
bool circle_from_3_points(const point_t *a, const point_t *b, const point_t *c, circle_t *out)
{
    SYSTEM_ASSERT(a != NULL);
    SYSTEM_ASSERT(b != NULL);
    SYSTEM_ASSERT(c != NULL);
    SYSTEM_ASSERT(out != NULL);
    distance_t yDelta_a = b->y - a->y;
    distance_t xDelta_a = b->x - a->x;
    distance_t yDelta_b = c->y - b->y;
    distance_t xDelta_b = c->x - b->x;
    float aSlope = yDelta_a / xDelta_a;
    float bSlope = yDelta_b / xDelta_b;

    if (!distance_null(xDelta_a) && !distance_null(xDelta_b) && !distance_null(bSlope - aSlope) &&
        !distance_null(aSlope))
    {
        out->c.x =
            (aSlope * bSlope * (a->y - c->y) + bSlope * (a->x + b->x) - aSlope * (b->x + c->x)) /
            (2 * (bSlope - aSlope));
        out->c.y = (-1 * (out->c.x - (a->x + b->x) / 2) / aSlope + (a->y + b->y) / 2);
    }
    else
    {
        return false;
    }
    out->r = point_distance(&out->c, a);
    return true;
}

/*
 * Equations trouvées sur
 * Explications sur : http://2000clicks.com/MathHelp/GeometryConicSectionCircleIntersection.aspx
 * Renvoie une segment avec les deux coordonées valant 0,0 si les cercles ne se croisent pas
 * ou sont coradiaux.
 */
bool circle_intersections(const circle_t *c0, const circle_t *c1, segment_t *out)
{
    SYSTEM_ASSERT(c0 != NULL);
    SYSTEM_ASSERT(c1 != NULL);
    SYSTEM_ASSERT(out != NULL);
    SYSTEM_ASSERT(c0->r >= 0.0f);
    SYSTEM_ASSERT(c1->r >= 0.0f);
    float x1 = c0->c.x;
    float y1 = c0->c.y;
    float r1 = c0->r;
    float x2 = c1->c.x;
    float y2 = c1->c.y;
    float r2 = c1->r;

    float x_diff = x2 - x1;
    float y_diff = y2 - y1;
    float sq_r1 = r1 * r1;
    float sq_r2 = r2 * r2;

    float radius_sum_square = (r1 + r2) * (r1 + r2);  /* somme des rayons au carré */
    float radius_diff_square = (r1 - r2) * (r1 - r2); /* différence des rayons au carré */
    float d2 =
        x_diff * x_diff + y_diff * y_diff; /* d est la distance entre les centres des cercles */
    float
        k; /* k vaut deux fois l'aire du triangle formé par les centres des deux cercles et un point d'intersection */
    float x_midpoint; /* Coordonnée en x du milieu des deux points d'intersection des cercles */
    float y_midpoint; /* Coordonnée en y du milieu des deux points d'intersection des cercles */
    float x_ratio_of_change;
    float y_ratio_of_change;

    /*On s'assure que la distance entre les centres des cercles se situe entre la différence de leurs rayons et la somme de
     * leurs rayons (inclus) */
    if (d2 > radius_diff_square && d2 <= radius_sum_square && !distance_null(d2))
    {
        k = sqrtf((radius_sum_square - d2) * (d2 - radius_diff_square)) / 2.f;
        x_midpoint = (x2 + x1) / 2.f + (x_diff * (sq_r1 - sq_r2)) / (2.f * d2);
        y_midpoint = (y2 + y1) / 2.f + (y_diff * (sq_r1 - sq_r2)) / (2.f * d2);
        x_ratio_of_change = y_diff * k / d2;
        y_ratio_of_change = x_diff * k / d2;
        out->a.x = x_midpoint + x_ratio_of_change;
        out->a.y = y_midpoint - y_ratio_of_change;
        out->b.x = x_midpoint - x_ratio_of_change;
        out->b.y = y_midpoint + y_ratio_of_change;
    }
    else
    {
        return false;
    }
    return true;
}

bool is_in_circle(const point_t *current, const circle_t *circle)
{
    SYSTEM_ASSERT(current != NULL);
    SYSTEM_ASSERT(circle != NULL);
    SYSTEM_ASSERT(circle->r >= 0.0f);
    return point_distance_square(current, &circle->c) <= SQUARE(circle->r);
}

/* ***************************************** Public callback functions definitions *************************************** */
