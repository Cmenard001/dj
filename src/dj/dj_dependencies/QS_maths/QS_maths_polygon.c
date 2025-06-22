#include "QS_maths_polygon.h"

#include <math.h>
#include <stdlib.h>

#define PROJ_TOLERANCE 3

bool GEOMETRY_is_in_square(int16_t x1, int16_t x2, int16_t y1, int16_t y2, GEOMETRY_point_t current)
{
    return current.x >= MIN(x1, x2) && current.x <= MAX(x1, x2) && current.y >= MIN(y1, y2) && current.y <= MAX(y1, y2);
}

bool GEOMETRY_is_in_rectangle(GEOMETRY_rectangle_t rect, GEOMETRY_point_t current)
{
    return current.x >= MIN(rect.x1, rect.x2) && current.x <= MAX(rect.x1, rect.x2) && current.y >= MIN(rect.y1, rect.y2)
           && current.y <= MAX(rect.y1, rect.y2);
}

/*
 *	http://fr.wikipedia.org/wiki/Quadrilat%C3%A8re
 *
    (yB ? yA)x ? (xB ? xA)y ? xAyB + xByA a même signe que (yB ? yA)xC ? (xB ? xA)yC ? xAyB + xByA ;
    (yC ? yB)x ? (xC ? xB)y ? xByC + xCyB a même signe que (yC ? yB)xD ? (xC ? xB)yD ? xByC + xCyB ;
    (yD ? yC)x ? (xD ? xC)y ? xCyD + xDyC a même signe que (yD ? yC)xA ? (xD ? xC)yA ? xCyD + xDyC ;
    (yA ? yD)x ? (xA ? xD)y ? xDyA + xAyD a même signe que (yA ? yD)xB ? (xA ? xD)yB ? xDyA + xAyD.

    Pour le test de chaque signe nous trouvons beaucoup de calcule en commun donc ils serront stockés dans les variables
 suivantes : temp1*x - temp2*y + temp3 a même signe que temp1*xC - temp2*Yc + temp3

 */
bool GEOMETRY_is_in_quadri(GEOMETRY_point_t points[4], GEOMETRY_point_t tested_point)
{
    int32_t temp1, temp2, temp3;

    temp1 = points[1].y - points[0].y;
    temp2 = points[1].x - points[0].x;
    temp3 = -points[0].x * points[1].y + points[1].x * points[0].y;
    if (SIGN(temp1 * tested_point.x - temp2 * tested_point.y + temp3)
        == SIGN(temp1 * points[2].x - temp2 * points[2].y + temp3))
    {
        temp1 = points[2].y - points[1].y;
        temp2 = points[2].x - points[1].x;
        temp3 = -points[1].x * points[2].y + points[2].x * points[1].y;
        if (SIGN(temp1 * tested_point.x - temp2 * tested_point.y + temp3)
            == SIGN(temp1 * points[3].x - temp2 * points[3].y + temp3))
        {
            temp1 = points[3].y - points[2].y;
            temp2 = points[3].x - points[2].x;
            temp3 = -points[2].x * points[3].y + points[3].x * points[2].y;
            if (SIGN(temp1 * tested_point.x - temp2 * tested_point.y + temp3)
                == SIGN(temp1 * points[0].x - temp2 * points[0].y + temp3))
            {
                temp1 = points[0].y - points[3].y;
                temp2 = points[0].x - points[3].x;
                temp3 = -points[3].x * points[0].y + points[0].x * points[3].y;
                if (SIGN(temp1 * tested_point.x - temp2 * tested_point.y + temp3)
                    == SIGN(temp1 * points[1].x - temp2 * points[1].y + temp3))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief GEOMETRY_is_in_polygon
 * @param polygon Les points du polygone
 * @param nb_summits Le nombre de sommets
 * @param tested_point Le point que l'on veut tester si il est dans le polygone
 * @param ref_point Un point dont on est sur qu'il est en dehors du polygone (par exemple en dehors du terrain
 * @return
 */
bool GEOMETRY_is_in_polygon(GEOMETRY_point_t polygon[],
                            uint8_t nb_summits,
                            GEOMETRY_point_t tested_point,
                            GEOMETRY_point_t out_point,
                            uint8_t *stock_intersections)
{
    uint8_t nb_intersections = 0;
    uint8_t i;
    // segment de référence dont une extrémité (x=3000, y=4000) est à l'extérieur du polygone
    GEOMETRY_segment_t seg1 = {tested_point, out_point};

    // on compte le nombre d'intersection avec chaque coté du polygone
    for (i = 0; i < nb_summits; i++)
    {
        GEOMETRY_segment_t seg2 = {polygon[i], polygon[(i + 1) % nb_summits]};
        if (GEOMETRY_segments_intersects(seg1, seg2))
        {
            // On vérifie que le premier point du segment 2 n'est sur le segment 1
            // Pour ne pas compter deux fois la même intersection si le segment 2
            // Passe par un sommet du polygone
            if (GEOMETRY_point_on_segment(seg1, seg2.a) == false)
            {
                nb_intersections++;
            }
        }
    }

    if (stock_intersections != NULL)
    {
        *stock_intersections = nb_intersections;
    }

    // Le point est à l'intérieur du polygone si le nombre d'intersections avec chacun des côté du polygone est un nombre
    // impair. Si le nombre d'intersection est un nombre pair, le node est donc à l'extérieur du polygone.
    return (nb_intersections % 2 == 1);
}

bool GEOMETRY_is_strictly_in_polygon(GEOMETRY_point_t polygon[],
                                     uint8_t nb_summits,
                                     GEOMETRY_point_t tested_point,
                                     GEOMETRY_point_t out_point,
                                     uint8_t *stock_intersections)
{
    // Check if the point is on a edge of the polygon
    for (uint8_t i = 0; i < nb_summits - 1; i++)
    {
        GEOMETRY_segment_t seg = {polygon[i], polygon[i + 1]};
        if (GEOMETRY_point_on_segment(seg, tested_point))
        {
            return false;
        }
    }
    GEOMETRY_segment_t seg = {polygon[nb_summits - 1], polygon[0]};
    if (GEOMETRY_point_on_segment(seg, tested_point))
    {
        return false;
    }

    // Then check if the point is in the polygon
    return GEOMETRY_is_in_polygon(polygon, nb_summits, tested_point, out_point, stock_intersections);
}

bool GEOMETRY_polygon_intersects_segment(GEOMETRY_polygon_t *polygon, GEOMETRY_segment_t segment)
{
    // Check if the segment intersects with any of the polygon's edges
    for (uint16_t i = 0; i < polygon->nb_points - 1; i++)
    {
        GEOMETRY_segment_t polygon_segment = {polygon->points[i], polygon->points[i + 1]};
        if (GEOMETRY_segments_intersects(polygon_segment, segment))
        {
            return true;
        }
    }
    GEOMETRY_segment_t polygon_segment = {polygon->points[polygon->nb_points - 1], polygon->points[0]};
    return GEOMETRY_segments_intersects(polygon_segment, segment);
}

bool GEOMETRY_polygon_strictly_intersects_segment(GEOMETRY_polygon_t *polygon, GEOMETRY_segment_t segment)
{
    // Check if the segment intersects with any of the polygon's edges
    for (uint16_t i = 0; i < polygon->nb_points - 1; i++)
    {
        GEOMETRY_segment_t polygon_segment = {polygon->points[i], polygon->points[i + 1]};
        // Skip this polygon segment if an extremity of the segment is on the polygon segment
        if (GEOMETRY_segments_intersects(polygon_segment, segment) && !GEOMETRY_point_on_segment(polygon_segment, segment.a)
            && !GEOMETRY_point_on_segment(polygon_segment, segment.b))
        {
            return true;
        }
    }
    GEOMETRY_segment_t polygon_segment = {polygon->points[polygon->nb_points - 1], polygon->points[0]};
    return GEOMETRY_segments_intersects(polygon_segment, segment) && !GEOMETRY_point_on_segment(polygon_segment, segment.a)
           && !GEOMETRY_point_on_segment(polygon_segment, segment.b);
}

GEOMETRY_polygon_t *GEOMETRY_polygon_create(uint16_t nb_points)
{
    GEOMETRY_polygon_t *polygon = (GEOMETRY_polygon_t *)malloc(sizeof(GEOMETRY_polygon_t));
    polygon->nb_points = nb_points;
    polygon->points = (GEOMETRY_point_t *)malloc(nb_points * sizeof(GEOMETRY_point_t));
    return polygon;
}

void GEOMETRY_polygon_destroy(GEOMETRY_polygon_t *polygon)
{
    free(polygon->points);
    free(polygon);
}

int16_t GEOMETRY_polygon_contains_point(GEOMETRY_polygon_t *polygon, GEOMETRY_point_t point)
{
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        if (polygon->points[i].x == point.x && polygon->points[i].y == point.y)
        {
            return i;
        }
    }
    return -1;
}

GEOMETRY_point_t GEOMETRY_polygon_project_point(GEOMETRY_polygon_t *polygon, GEOMETRY_point_t point)
{
    GEOMETRY_point_t projected_point = {0, 0};
    uint32_t min_distance = UINT32_MAX;
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        uint16_t next_index = (i + 1) % polygon->nb_points;
        GEOMETRY_segment_t segment = {polygon->points[i], polygon->points[next_index]};
        GEOMETRY_point_t projection = GEOMETRY_proj_on_line(segment, point);
        uint32_t distance = GEOMETRY_distance_square(point, projection);
        if (distance < min_distance)
        {
            min_distance = distance;
            projected_point = projection;
        }
    }
    return projected_point;
}

void GEOMETRY_polygon_set_in_trigonometric_order(GEOMETRY_polygon_t *polygon)
{
    // First, we need to know if the polygon is in trigonometric order
    int32_t area = 0;

    // Compute the area of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        GEOMETRY_point_t point = polygon->points[i];
        GEOMETRY_point_t next_point = polygon->points[(i + 1) % polygon->nb_points];
        area += (point.x * next_point.y) - (point.y * next_point.x);
    }

    // If the area is negative, the polygon is in hourly order
    if (area < 0)
    {
        // The polygon is in hourly order
        // We need to reverse the order of the points
        for (uint16_t i = 0; i < polygon->nb_points / 2; i++)
        {
            GEOMETRY_point_t tmp = polygon->points[i];
            polygon->points[i] = polygon->points[polygon->nb_points - i - 1];
            polygon->points[polygon->nb_points - i - 1] = tmp;
        }
    }
}

bool GEOMETRY_polygon_fusion(GEOMETRY_polygon_t *polygon1,
                             GEOMETRY_polygon_t *polygon2,
                             GEOMETRY_polygon_t *out_polygon,
                             uint8_t nb_points_max_in_out_polygon)
{
    // Check if the polygons are in trigonometric order
    GEOMETRY_polygon_set_in_trigonometric_order(polygon1);
    GEOMETRY_polygon_set_in_trigonometric_order(polygon2);

    // Find a point of the first polygon that is not in the second polygon
    GEOMETRY_point_t point_to_add;
    bool at_least_one_point_not_in_polygon2 = false;
    for (uint16_t i = 0; i < polygon1->nb_points; i++)
    {
        point_to_add = polygon1->points[i];
        const GEOMETRY_point_t out_point = {-10000, -10000};
        if (GEOMETRY_is_in_polygon(polygon2->points, polygon2->nb_points, point_to_add, out_point, NULL) == false)
        {
            // We found a point that is not in the second polygon
            at_least_one_point_not_in_polygon2 = true;
            break;
        }
    }

    if (!at_least_one_point_not_in_polygon2)
    {
        // All points of the first polygon are in the second polygon
        // We can just copy the points of the second polygon
        if (polygon2->nb_points > nb_points_max_in_out_polygon)
        {
            // Not enough space in the output polygon
            return false;
        }
        for (uint16_t i = 0; i < polygon2->nb_points; i++)
        {
            out_polygon->points[i] = polygon2->points[i];
        }
        out_polygon->nb_points = polygon2->nb_points;
        return true;
    }

    out_polygon->nb_points = 0;
    uint16_t index_current_polygon = 0;
    GEOMETRY_polygon_t *current_polygon = polygon1;
    GEOMETRY_polygon_t *other_polygon = polygon2;
    do
    {
        // add the point found at last iteration in the polygon
        if (out_polygon->nb_points >= nb_points_max_in_out_polygon)
        {
            // Not enough space in the output polygon
            return false;
        }
        out_polygon->points[out_polygon->nb_points++] = point_to_add;

        // Increment the index of the current polygon to the next point
        index_current_polygon = (index_current_polygon + 1) % current_polygon->nb_points;
        // Get the segment to the next point of the current polygon
        GEOMETRY_point_t next_point = current_polygon->points[index_current_polygon];
        GEOMETRY_segment_t segment = {.a = point_to_add, .b = next_point};
        // Check if the segment intersect a segment of the other polygon
        // If the segment intersects the other polygon two times or more, we need to find the closest point
        uint32_t closest_distance = UINT32_MAX;
        uint16_t closest_index = 0;
        bool at_least_one_intersection = false;
        GEOMETRY_point_t closest_intersection = {0, 0};
        for (uint16_t i = 0; i < other_polygon->nb_points; i++)
        {
            GEOMETRY_segment_t other_segment
                = {.a = other_polygon->points[i], .b = other_polygon->points[(i + 1) % other_polygon->nb_points]};
            GEOMETRY_point_t current_intersection;
            if (GEOMETRY_segments_get_intersection(segment, other_segment, &current_intersection))
            {
                // Check if the segments are colinear
                bool colinear = GEOMETRY_segments_parallel(segment, other_segment);
                if (colinear)
                {
                    // Switch to the other polygon
                    GEOMETRY_polygon_t *tmp = current_polygon;
                    current_polygon = other_polygon;
                    other_polygon = tmp;
                    // Set the new index of the new current polygon
                    index_current_polygon = (i + 1) % current_polygon->nb_points;
                    // The point to add is the next point of the new current polygon
                    next_point = current_polygon->points[index_current_polygon];
                    // The point to add is not an intersection point
                    at_least_one_intersection = false;
                    // We need to break the loop because we have already added the point
                    break;
                }
                uint32_t distance = GEOMETRY_distance_square(point_to_add, current_intersection);
                // Filter the distance of 0 to because it mean that intersection point is the same as last intersection point
                if (distance < closest_distance && distance != 0)
                {
                    at_least_one_intersection = true;
                    closest_distance = distance;
                    closest_index = i;
                    closest_intersection = current_intersection;
                }
            }
        }

        if (at_least_one_intersection)
        {
            // We have at least one intersection
            // Add the closest intersection point to the output polygon
            point_to_add = closest_intersection;
            // Switch to the other polygon
            GEOMETRY_polygon_t *tmp = current_polygon;
            current_polygon = other_polygon;
            other_polygon = tmp;
            // Set the new index of the new current polygon
            index_current_polygon = closest_index;
        }
        else
        {
            // No intersection, we can add the next point of the current polygon
            point_to_add = next_point;
        }

    } while (GEOMETRY_point_compare(point_to_add, out_polygon->points[0]) == false);

    return true;
}

GEOMETRY_point_t GEOMETRY_polygon_find_closest_point(GEOMETRY_polygon_t *polygon, GEOMETRY_point_t point)
{
    GEOMETRY_point_t closest_point;
    uint32_t min_distance = UINT32_MAX;

    // Shearch in the submit of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        uint32_t distance = GEOMETRY_distance_square(polygon->points[i], point);
        if (distance < min_distance)
        {
            min_distance = distance;
            closest_point = polygon->points[i];
        }
    }

    // Shearch in orthogonal projections of the points on the edges of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        uint16_t next_index = (i + 1) % polygon->nb_points;
        GEOMETRY_segment_t segment = {polygon->points[i], polygon->points[next_index]};
        GEOMETRY_point_t projection = GEOMETRY_proj_on_line(segment, point);
        // Check if the projection is on the segment
        if (MIN(segment.a.x, segment.b.x) <= projection.x && projection.x <= MAX(segment.a.x, segment.b.x)
            && MIN(segment.a.y, segment.b.y) <= projection.y && projection.y <= MAX(segment.a.y, segment.b.y))
        {
            uint32_t distance = GEOMETRY_distance_square(projection, point);
            if (distance < min_distance)
            {
                min_distance = distance;
                closest_point = projection;
            }
        }
    }

    return closest_point;
}

void GEOMETRY_polygon_create_regular(GEOMETRY_polygon_t *polygon,
                                     uint32_t nb_summit,
                                     uint16_t rayon,
                                     GEOMETRY_point_t *position)
{

    float angle_inc_rad = (M_PI * 2) / nb_summit;
    float angle_rad = angle_inc_rad / 2;

    uint32_t index = nb_summit;
    while (index--)
    {
        int32_t x = cosf(angle_rad) * rayon + position->x;
        int32_t y = sinf(angle_rad) * rayon + position->y;

        polygon->points[index].x = x;
        polygon->points[index].y = y;

        angle_rad += angle_inc_rad;
    }
}
