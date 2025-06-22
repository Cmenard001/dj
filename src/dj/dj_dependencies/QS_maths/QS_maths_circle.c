#include "QS_maths_circle.h"

#include <math.h>

GEOMETRY_circle_t GEOMETRY_circle_from_diameter(GEOMETRY_segment_t diameter)
{
    GEOMETRY_circle_t c;
    c.c = GEOMETRY_segment_middle(diameter);
    c.r = GEOMETRY_distance(c.c, diameter.a);
    return c;
}

// https://stackoverflow.com/questions/4103405/what-is-the-algorithm-for-finding-the-center-of-a-circle-from-three-points
GEOMETRY_point_t GEOMETRY_circle_from_3_points(GEOMETRY_point_t A, GEOMETRY_point_t B, GEOMETRY_point_t C)
{
    GEOMETRY_point_t center;

    float yDelta_a = B.y - A.y;
    float xDelta_a = B.x - A.x;
    float yDelta_b = C.y - B.y;
    float xDelta_b = C.x - B.x;
    float aSlope = yDelta_a / xDelta_a;
    float bSlope = yDelta_b / xDelta_b;

    if (xDelta_a != 0 && xDelta_b != 0 && bSlope - aSlope != 0 && aSlope != 0)
    {
        center.x = (aSlope * bSlope * (float)(A.y - C.y) + bSlope * (float)(A.x + B.x) - aSlope * (float)(B.x + C.x))
                   / ((float)2 * (bSlope - aSlope));
        center.y = (float)-1 * ((float)center.x - (float)(A.x + B.x) / (float)2) / aSlope + (float)(A.y + B.y) / (float)2;
    }
    else
    {
        center.x = 0;
        center.y = 0;
    }
    return center;
}

/*
 * Equations trouvées sur
 * Explications sur : http://2000clicks.com/MathHelp/GeometryConicSectionCircleIntersection.aspx
 * Renvoie une segment avec les deux coordonées valant 0,0 si les cercles ne se croisent pas
 * ou sont coradiaux.
 */
GEOMETRY_segment_t GEOMETRY_circle_intersections(GEOMETRY_circle_t c1, GEOMETRY_circle_t c2)
{
    GEOMETRY_segment_t points;
    float x1 = c1.c.x;
    float y1 = c1.c.y;
    float r1 = c1.r;
    float x2 = c2.c.x;
    float y2 = c2.c.y;
    float r2 = c2.r;

    float x_diff = x2 - x1;
    float y_diff = y2 - y1;
    float sq_r1 = r1 * r1;
    float sq_r2 = r2 * r2;

    float radius_sum_square = (r1 + r2) * (r1 + r2);  /* somme des rayons au carré */
    float radius_diff_square = (r1 - r2) * (r1 - r2); /* différence des rayons au carré */
    float d2 = x_diff * x_diff + y_diff * y_diff;     /* d est la distance entre les centres des cercles */
    float k; /* k vaut deux fois l'aire du triangle formé par les centres des deux cercles et un point d'intersection */
    float x_midpoint; /* Coordonnée en x du milieu des deux points d'intersection des cercles */
    float y_midpoint; /* Coordonnée en y du milieu des deux points d'intersection des cercles */
    float x_ratio_of_change;
    float y_ratio_of_change;

    /*On s'assure que la distance entre les centres des cercles se situe entre la différence de leurs rayons et la somme de
     * leurs rayons (inclus) */
    if (d2 > radius_diff_square && d2 <= radius_sum_square && d2 != 0)
    {
        k = sqrt((radius_sum_square - d2) * (d2 - radius_diff_square)) / 2.;
        x_midpoint = (x2 + x1) / 2. + (x_diff * (sq_r1 - sq_r2)) / (2. * d2);
        y_midpoint = (y2 + y1) / 2. + (y_diff * (sq_r1 - sq_r2)) / (2. * d2);
        x_ratio_of_change = y_diff * k / d2;
        y_ratio_of_change = x_diff * k / d2;
        points.a.x = x_midpoint + x_ratio_of_change;
        points.a.y = y_midpoint - y_ratio_of_change;
        points.b.x = x_midpoint - x_ratio_of_change;
        points.b.y = y_midpoint + y_ratio_of_change;
    }
    else
    {
        points.a.x = 0;
        points.a.y = 0;
        points.b.x = 0;
        points.b.y = 0;
    }

    return points;
}

bool GEOMETRY_is_in_circle(GEOMETRY_point_t current, GEOMETRY_circle_t circle)
{
    return SQUARE((uint32_t){current.x - circle.c.x}) + SQUARE((uint32_t){current.y - circle.c.y})
           <= SQUARE((uint32_t){circle.r});
}
