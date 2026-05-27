/**
 * @file polygon.c
 * @brief Polygon utilities
 * @author Arnaud
 * Refactored by Cyprien Ménard
 * @date 12/10/2025
 * @see polygon.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "polygon.h"
#include "system/assert/system_assert.h"
#include "utils/macros/macros.h"
#include "utils/maths/angle/angle.h"
#include "utils/maths/point/point.h"
#include "utils/maths/position/position.h"
#include "utils/maths/segment/segment.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Tolerance for projection on polygon edges
 */
#define PROJ_TOLERANCE 3

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/**
 * @brief Oversize a corner of the polygon
 * @details This function will add nb_angle_points points in the output polygon
 * @param[in] original_polygon : the original polygon
 * @param[out] output_polygon : the polygon with the oversize
 * @param[in] index : the index of the point to oversize
 * @param[in] nb_angle_points : number of points to add for this corner
 * @param[in] offset : offset to apply (can be negative to shrink)
 */
static void oversize_corner(polygon_t *original_polygon,
                            polygon_t *output_polygon,
                            uint16_t index,
                            uint8_t nb_angle_points,
                            distance_t offset);

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

static void oversize_corner(polygon_t *original_polygon,
                            polygon_t *output_polygon,
                            uint16_t index,
                            uint8_t nb_angle_points,
                            distance_t offset)
{
    point_t point = original_polygon->points[index];
    point_t next_point = original_polygon->points[(index + 1) % original_polygon->nb_points];
    point_t prev_point =
        original_polygon
            ->points[(index + original_polygon->nb_points - 1) % original_polygon->nb_points];

    // Compute the angle of each segment
    angle_t angle_next, angle_prev;
    viewing_angle(&point, &next_point, &angle_next);
    viewing_angle(&point, &prev_point, &angle_prev);
    angle_t angle_total = modulo_angle(angle_next - angle_prev - PI);

    // Compute the number of points to add
    uint16_t nb_points = 0;
    if (angle_total < 0)
    {
        // Concave corner, only add one point
        nb_points = 1;
    }
    else
    {
        nb_points = nb_angle_points;
    }

    // Compute angle increment between each point
    angle_t angle_increment = angle_total / (float)nb_points;

    // Compute the first angle
    angle_t current_angle = modulo_angle(angle_prev + PI / 2.0f + angle_increment / 2.0f);

    // Compute all the oversized points
    for (uint16_t point_index = 0; point_index < nb_points; point_index++)
    {
        // Compute the distance between the point and the new point
        distance_t distance = offset / cosf(angle_increment / 2.0f);

        // Compute the new point using position helper
        position_t temp_pos = {.point = point, .angle = current_angle};
        point_t new_point;
        calculate_point_from_distance_and_position(&temp_pos, distance, &new_point);

        // Add the point to the output polygon
        output_polygon->points[output_polygon->nb_points] = new_point;
        output_polygon->nb_points++;

        // Update the current angle
        current_angle = modulo_angle(current_angle + angle_increment);
    }
}

/* ********************************************** Public functions definitions ******************************************* */

bool is_in_rectangle(const rectangle_t *rect, const point_t *current)
{
    SYSTEM_ASSERT(rect != NULL);
    SYSTEM_ASSERT(current != NULL);
    return current->x >= MIN(rect->p1.x, rect->p2.x) && current->x <= MAX(rect->p1.x, rect->p2.x) &&
           current->y >= MIN(rect->p1.y, rect->p2.y) && current->y <= MAX(rect->p1.y, rect->p2.y);
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
bool is_in_quadri(const point_t points[4], const point_t *tested_point)
{
    SYSTEM_ASSERT(points != NULL);
    SYSTEM_ASSERT(tested_point != NULL);
    distance_t temp1, temp2, temp3;

    temp1 = points[1].y - points[0].y;
    temp2 = points[1].x - points[0].x;
    temp3 = -points[0].x * points[1].y + points[1].x * points[0].y;
    if (SIGN(temp1 * tested_point->x - temp2 * tested_point->y + temp3) ==
        SIGN(temp1 * points[2].x - temp2 * points[2].y + temp3))
    {
        temp1 = points[2].y - points[1].y;
        temp2 = points[2].x - points[1].x;
        temp3 = -points[1].x * points[2].y + points[2].x * points[1].y;
        if (SIGN(temp1 * tested_point->x - temp2 * tested_point->y + temp3) ==
            SIGN(temp1 * points[3].x - temp2 * points[3].y + temp3))
        {
            temp1 = points[3].y - points[2].y;
            temp2 = points[3].x - points[2].x;
            temp3 = -points[2].x * points[3].y + points[3].x * points[2].y;
            if (SIGN(temp1 * tested_point->x - temp2 * tested_point->y + temp3) ==
                SIGN(temp1 * points[0].x - temp2 * points[0].y + temp3))
            {
                temp1 = points[0].y - points[3].y;
                temp2 = points[0].x - points[3].x;
                temp3 = -points[3].x * points[0].y + points[0].x * points[3].y;
                if (SIGN(temp1 * tested_point->x - temp2 * tested_point->y + temp3) ==
                    SIGN(temp1 * points[1].x - temp2 * points[1].y + temp3))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief is_in_polygon
 * @param polygon Les points du polygone
 * @param nb_summits Le nombre de sommets
 * @param tested_point Le point que l'on veut tester si il est dans le polygone
 * @param ref_point Un point dont on est sur qu'il est en dehors du polygone (par exemple en dehors du terrain
 * @return
 */
bool is_in_polygon(const point_t polygon[],
                   uint8_t nb_summits,
                   const point_t *tested_point,
                   const point_t *out_point,
                   uint8_t *stock_intersections)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(tested_point != NULL);
    SYSTEM_ASSERT(out_point != NULL);
    SYSTEM_ASSERT(nb_summits > 0);
    uint8_t nb_intersections = 0;
    uint8_t i;
    // segment de référence dont une extrémité (x=3000, y=4000) est à l'extérieur du polygone
    segment_t seg1 = {*tested_point, *out_point};

    // on compte le nombre d'intersection avec chaque coté du polygone
    for (i = 0; i < nb_summits; i++)
    {
        segment_t seg2 = {polygon[i], polygon[(i + 1) % nb_summits]};
        if (segments_intersects(&seg1, &seg2))
        {
            // On vérifie que le premier point du segment 2 n'est sur le segment 1
            // Pour ne pas compter deux fois la même intersection si le segment 2
            // Passe par un sommet du polygone
            if (point_on_segment(&seg1, &seg2.a) == false)
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

bool is_strictly_in_polygon(const point_t polygon[],
                            uint8_t nb_summits,
                            const point_t *tested_point,
                            const point_t *out_point,
                            uint8_t *stock_intersections)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(tested_point != NULL);
    SYSTEM_ASSERT(out_point != NULL);
    SYSTEM_ASSERT(nb_summits > 0);
    // Check if the point is on a edge of the polygon
    for (uint8_t i = 0; i < nb_summits - 1; i++)
    {
        segment_t seg = {polygon[i], polygon[i + 1]};
        if (point_on_segment(&seg, tested_point))
        {
            return false;
        }
    }
    segment_t seg = {polygon[nb_summits - 1], polygon[0]};
    if (point_on_segment(&seg, tested_point))
    {
        return false;
    }

    // Then check if the point is in the polygon
    return is_in_polygon(polygon, nb_summits, tested_point, out_point, stock_intersections);
}

void polygon_segment_get_intersection(const point_t polygon[],
                                      uint16_t nb_summits,
                                      const segment_t *segment,
                                      point_t *intersection_points,
                                      uint16_t *nb_intersection_points)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(segment != NULL);
    SYSTEM_ASSERT(intersection_points != NULL);
    SYSTEM_ASSERT(nb_intersection_points != NULL);
    SYSTEM_ASSERT(nb_summits > 0);
    // Get the intersection points between the segment and the polygon
    uint16_t nb_intersections = 0;
    for (uint16_t i = 0; i < nb_summits - 1; i++)
    {
        segment_t seg = {polygon[i], polygon[i + 1]};
        point_t intersection;
        if (segments_intersects(&seg, segment))
        {
            segments_get_intersection(&seg, segment, &intersection);
            intersection_points[nb_intersections] = intersection;
            nb_intersections++;
        }
    }
    segment_t seg = {polygon[nb_summits - 1], polygon[0]};
    point_t intersection;
    if (segments_intersects(&seg, segment))
    {
        segments_get_intersection(&seg, segment, &intersection);
        intersection_points[nb_intersections] = intersection;
        nb_intersections++;
    }
    *nb_intersection_points = nb_intersections;
}

void polygon_strict_segment_get_intersection(const point_t polygon[],
                                             uint16_t nb_summits,
                                             const segment_t *segment,
                                             point_t *intersection_points,
                                             uint16_t *nb_intersection_points)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(segment != NULL);
    SYSTEM_ASSERT(intersection_points != NULL);
    SYSTEM_ASSERT(nb_intersection_points != NULL);
    SYSTEM_ASSERT(nb_summits > 0);
    // Get the intersection points between the segment and the polygon
    uint16_t nb_intersections = 0;
    for (uint16_t i = 0; i < nb_summits - 1; i++)
    {
        segment_t seg = {polygon[i], polygon[i + 1]};
        point_t intersection;
        if (segments_intersects(&seg, segment) && !point_on_segment(&seg, &segment->a) &&
            !point_on_segment(&seg, &segment->b))
        {
            segments_get_intersection(&seg, segment, &intersection);
            intersection_points[nb_intersections] = intersection;
            nb_intersections++;
        }
    }
    segment_t seg = {polygon[nb_summits - 1], polygon[0]};
    point_t intersection;
    if (segments_intersects(&seg, segment) && !point_on_segment(&seg, &segment->a) &&
        !point_on_segment(&seg, &segment->b))
    {
        segments_get_intersection(&seg, segment, &intersection);
        intersection_points[nb_intersections] = intersection;
        nb_intersections++;
    }
    *nb_intersection_points = nb_intersections;
}

bool polygon_intersects_segment(const polygon_t *polygon, const segment_t *segment)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(segment != NULL);
    SYSTEM_ASSERT(polygon->nb_points > 0);
    // Check if the segment intersects with any of the polygon's edges
    for (uint16_t i = 0; i < polygon->nb_points - 1; i++)
    {
        segment_t polygon_segment = {polygon->points[i], polygon->points[i + 1]};
        if (segments_intersects(&polygon_segment, segment))
        {
            return true;
        }
    }
    segment_t polygon_segment = {polygon->points[polygon->nb_points - 1], polygon->points[0]};
    return segments_intersects(&polygon_segment, segment);
}

bool polygon_strictly_intersects_segment(const polygon_t *polygon, const segment_t *segment)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(segment != NULL);
    SYSTEM_ASSERT(polygon->nb_points > 0);
    // Check if the segment intersects with any of the polygon's edges
    for (uint16_t i = 0; i < polygon->nb_points - 1; i++)
    {
        segment_t polygon_segment = {polygon->points[i], polygon->points[i + 1]};
        // Skip this polygon segment if an extremity of the segment is on the polygon segment
        if (segments_intersects(&polygon_segment, segment) &&
            !point_on_segment(&polygon_segment, &segment->a) &&
            !point_on_segment(&polygon_segment, &segment->b))
        {
            return true;
        }
    }
    segment_t polygon_segment = {polygon->points[polygon->nb_points - 1], polygon->points[0]};
    return segments_intersects(&polygon_segment, segment) &&
           !point_on_segment(&polygon_segment, &segment->a) &&
           !point_on_segment(&polygon_segment, &segment->b);
}

polygon_t *polygon_create(uint16_t nb_points)
{
    SYSTEM_ASSERT(nb_points > 0);
    polygon_t *polygon = (polygon_t *)malloc(sizeof(polygon_t));
    SYSTEM_ASSERT(polygon != NULL);
    polygon->nb_points = nb_points;
    polygon->points = (point_t *)malloc(nb_points * sizeof(point_t));
    SYSTEM_ASSERT(polygon->points != NULL);
    return polygon;
}

void polygon_destroy(polygon_t *polygon)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(polygon->points != NULL);
    free(polygon->points);
    free(polygon);
}

int16_t polygon_contains_point(const polygon_t *polygon, const point_t *point)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(point != NULL);
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        if (point_compare(&polygon->points[i], point))
        {
            return (int16_t)i;
        }
    }
    return -1;
}

void polygon_project_point(const polygon_t *polygon, const point_t *point, point_t *projected)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(point != NULL);
    SYSTEM_ASSERT(projected != NULL);
    SYSTEM_ASSERT(polygon->nb_points > 0);
    *projected = (point_t){0, 0};
    distance_t min_distance = FLT_MAX;
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        uint16_t next_index = (uint16_t)((i + 1) % polygon->nb_points);
        segment_t segment = {polygon->points[i], polygon->points[next_index]};
        point_t projection;
        proj_on_line(&segment, point, &projection);
        float distance = point_distance_square(point, &projection);
        if (distance < min_distance)
        {
            min_distance = distance;
            *projected = projection;
        }
    }
}

bool polygon_project_point_with_angle(const polygon_t *polygon,
                                      const point_t *point,
                                      angle_t angle,
                                      point_t *projected)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(point != NULL);
    SYSTEM_ASSERT(projected != NULL);
    SYSTEM_ASSERT(polygon->nb_points > 0);
    float min_distance_sq = FLT_MAX;
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        uint16_t next_index = (uint16_t)((i + 1) % polygon->nb_points);
        segment_t segment = {polygon->points[i], polygon->points[next_index]};
        // Project the point on the segment based on the angle
        position_t pos = {.point = *point, .angle = angle};
        point_t projection;
        bool success = project_position_on_segment(&pos, &segment, &projection);
        if (success)
        {
            float distance_sq = point_distance_square(point, &projection);
            if (distance_sq < min_distance_sq)
            {
                min_distance_sq = distance_sq;
                *projected = projection;
            }
        }
    }
    return min_distance_sq != FLT_MAX;
}

void polygon_set_in_trigonometric_order(polygon_t *polygon)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(polygon->nb_points > 0);
    // First, we need to know if the polygon is in trigonometric order
    float area = 0;

    // Compute the area of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        point_t point = polygon->points[i];
        point_t next_point = polygon->points[(i + 1) % polygon->nb_points];
        area += (point.x * next_point.y) - (point.y * next_point.x);
    }

    // If the area is negative, the polygon is in hourly order
    if (area < 0)
    {
        // The polygon is in hourly order
        // We need to reverse the order of the points
        for (uint16_t i = 0; i < polygon->nb_points / 2; i++)
        {
            point_t tmp = polygon->points[i];
            polygon->points[i] = polygon->points[polygon->nb_points - i - 1];
            polygon->points[polygon->nb_points - i - 1] = tmp;
        }
    }
}

bool polygon_fusion(polygon_t *polygon1,
                    polygon_t *polygon2,
                    polygon_t *out_polygon,
                    uint8_t nb_points_max_in_out_polygon)
{
    SYSTEM_ASSERT(polygon1 != NULL);
    SYSTEM_ASSERT(polygon2 != NULL);
    SYSTEM_ASSERT(out_polygon != NULL);
    SYSTEM_ASSERT(nb_points_max_in_out_polygon > 0);
    // Check if the polygons are in trigonometric order
    polygon_set_in_trigonometric_order(polygon1);
    polygon_set_in_trigonometric_order(polygon2);

    // Find a point of the first polygon that is not in the second polygon
    point_t point_to_add;
    bool at_least_one_point_not_in_polygon2 = false;
    for (uint16_t i = 0; i < polygon1->nb_points; i++)
    {
        point_to_add = polygon1->points[i];
        const point_t out_point = {-10000, -10000};
        if (is_in_polygon(polygon2->points,
                          (uint8_t)polygon2->nb_points,
                          &point_to_add,
                          (point_t *)&out_point,
                          NULL) == false)
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
    polygon_t *current_polygon = polygon1;
    polygon_t *other_polygon = polygon2;
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
        index_current_polygon =
            (uint16_t)((index_current_polygon + 1) % current_polygon->nb_points);
        // Get the segment to the next point of the current polygon
        point_t next_point = current_polygon->points[index_current_polygon];
        segment_t segment = {.a = point_to_add, .b = next_point};
        // Check if the segment intersect a segment of the other polygon
        // If the segment intersects the other polygon two times or more, we need to find the closest point
        float closest_distance = FLT_MAX;
        uint16_t closest_index = 0;
        bool at_least_one_intersection = false;
        point_t closest_intersection = {0, 0};
        for (uint16_t i = 0; i < other_polygon->nb_points; i++)
        {
            segment_t other_segment = {
                .a = other_polygon->points[i],
                .b = other_polygon->points[(i + 1) % other_polygon->nb_points]};
            point_t current_intersection;
            if (segments_get_intersection(&segment, &other_segment, &current_intersection))
            {
                // Check if the segments are colinear
                bool colinear = segments_parallel(&segment, &other_segment);
                if (colinear)
                {
                    // Switch to the other polygon
                    polygon_t *tmp = current_polygon;
                    current_polygon = other_polygon;
                    other_polygon = tmp;
                    // Set the new index of the new current polygon
                    index_current_polygon = (uint16_t)((i + 1) % current_polygon->nb_points);
                    // The point to add is the next point of the new current polygon
                    next_point = current_polygon->points[index_current_polygon];
                    // The point to add is not an intersection point
                    at_least_one_intersection = false;
                    // We need to break the loop because we have already added the point
                    break;
                }
                distance_t distance = point_distance_square(&point_to_add, &current_intersection);
                // Filter the distance of 0 to because it mean that intersection point is the same as last intersection point
                if (distance < closest_distance && !distance_null(distance))
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
            polygon_t *tmp = current_polygon;
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

    } while (point_compare(&point_to_add, &out_polygon->points[0]) == false);

    return true;
}

void polygon_find_closest_point(const polygon_t *polygon, const point_t *point, point_t *closest)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(point != NULL);
    SYSTEM_ASSERT(closest != NULL);
    SYSTEM_ASSERT(polygon->nb_points > 0);
    *closest = polygon->points[0];
    float min_distance = FLT_MAX;

    // Shearch in the submit of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        float distance = point_distance_square(&polygon->points[i], point);
        if (distance < min_distance)
        {
            min_distance = distance;
            *closest = polygon->points[i];
        }
    }

    // Shearch in orthogonal projections of the points on the edges of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        uint16_t next_index = (uint16_t)((i + 1) % polygon->nb_points);
        segment_t segment = {polygon->points[i], polygon->points[next_index]};
        point_t projection;
        proj_on_line(&segment, point, &projection);
        // Check if the projection is on the segment
        if (MIN(segment.a.x, segment.b.x) <= projection.x &&
            projection.x <= MAX(segment.a.x, segment.b.x) &&
            MIN(segment.a.y, segment.b.y) <= projection.y &&
            projection.y <= MAX(segment.a.y, segment.b.y))
        {
            float distance = point_distance_square(&projection, point);
            if (distance < min_distance)
            {
                min_distance = distance;
                *closest = projection;
            }
        }
    }
}

void polygon_create_regular(polygon_t *polygon,
                            uint32_t nb_summit,
                            distance_t radius,
                            const point_t *position)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(position != NULL);
    SYSTEM_ASSERT(nb_summit >= 3);
    SYSTEM_ASSERT(radius > 0.f);

    angle_t angle_inc_rad = (PI * 2.f) / (float)nb_summit;
    angle_t angle_rad = angle_inc_rad / 2;

    uint32_t index = nb_summit;
    while (index--)
    {
        distance_t x = cosf(angle_rad) * radius + position->x;
        distance_t y = sinf(angle_rad) * radius + position->y;
        polygon->points[index].x = x;
        polygon->points[index].y = y;

        angle_rad += angle_inc_rad;
    }
    polygon->nb_points = (uint16_t)nb_summit;
}

bool polygon_oversize(polygon_t *polygon,
                      polygon_t *output_polygon,
                      uint8_t nb_angle_points,
                      uint16_t max_points,
                      distance_t offset)
{
    SYSTEM_ASSERT(polygon != NULL);
    SYSTEM_ASSERT(output_polygon != NULL);
    SYSTEM_ASSERT(max_points > 0);
    if (nb_angle_points == 0)
    {
        // No oversizing requested
        return true;
    }

    // Output polygon
    // At the beginning, the output polygon is empty
    // The output polygon will be filled with the oversize points
    output_polygon->nb_points = 0;

    // Make sure the input polygon is set in trigonometric order
    polygon_set_in_trigonometric_order(polygon);

    // Make an oversize of each point of the polygon
    for (uint16_t i = 0; i < polygon->nb_points; i++)
    {
        // Check if we have enough space in the output polygon
        if (output_polygon->nb_points + nb_angle_points > max_points)
        {
            // Not enough space in the output polygon
            return false;
        }

        // oversize the corner
        oversize_corner(polygon, output_polygon, i, nb_angle_points, offset);
    }

    return true;
}

bool polygon_convex_hull(const point_t *points,
                         uint16_t nb_points,
                         polygon_t *out_polygon,
                         uint16_t max_points)
{
    SYSTEM_ASSERT(points != NULL);
    SYSTEM_ASSERT(out_polygon != NULL);
    SYSTEM_ASSERT(max_points > 0);

    // Besoin d'au moins 3 points pour former un polygone
    if (nb_points < 3)
    {
        return false;
    }

    // Trouver le point le plus à gauche (et le plus bas en cas d'égalité)
    uint16_t leftmost_index = 0;
    for (uint16_t i = 1; i < nb_points; i++)
    {
        if (points[i].x < points[leftmost_index].x ||
            (points[i].x == points[leftmost_index].x && points[i].y < points[leftmost_index].y))
        {
            leftmost_index = i;
        }
    }

    // Initialiser l'enveloppe convexe
    out_polygon->nb_points = 0;

    // Point de départ
    uint16_t current_index = leftmost_index;

    // Algorithme de Jarvis March (Gift Wrapping)
    do
    {
        // Vérifier qu'on a assez de place
        if (out_polygon->nb_points >= max_points)
        {
            return false;
        }

        // Ajouter le point actuel à l'enveloppe convexe
        out_polygon->points[out_polygon->nb_points++] = points[current_index];

        // Trouver le point suivant
        // On cherche le point qui est le plus "à gauche" par rapport au point actuel
        uint16_t next_index = 0;
        for (uint16_t i = 0; i < nb_points; i++)
        {
            if (i == current_index)
            {
                continue;
            }

            // Si next_index == current_index, initialiser avec i
            if (next_index == current_index)
            {
                next_index = i;
                continue;
            }

            // Calculer le produit vectoriel
            // cross > 0 : i est à gauche du segment current->next
            // cross < 0 : i est à droite
            // cross = 0 : i est aligné
            float cross =
                point_cross_product(&points[current_index], &points[next_index], &points[i]);

            // Si cross > 0, le point i est à gauche du segment current->next
            // On prend ce point car il est plus "à gauche"
            // Si cross = 0, les points sont alignés, on prend le plus éloigné pour éviter les points colinéaires
            if (cross > 0.0f)
            {
                next_index = i;
            }
            else if (cross == 0.0f)
            {
                // Points alignés : prendre le plus éloigné
                if (point_distance_square(&points[current_index], &points[i]) >
                    point_distance_square(&points[current_index], &points[next_index]))
                {
                    next_index = i;
                }
            }
        }

        current_index = next_index;

    } while (current_index != leftmost_index);

    // L'algorithme garantit que le polygone est dans l'ordre trigonométrique
    // car on part du point le plus à gauche et on tourne dans le sens anti-horaire

    return true;
}

bool polygon_from_cluster(const point_t *points,
                          uint16_t nb_points,
                          polygon_t *out_polygon,
                          uint16_t max_points,
                          polygon_from_cluster_work_data_t *work_data)
{
    if (nb_points < 3 || max_points < 3)
    {
        return false;
    }

    // 1. Trouver le centre de masse du cluster
    point_t c = {0, 0};
    for (uint16_t i = 0; i < nb_points; i++)
    {
        c.x += points[i].x;
        c.y += points[i].y;
    }
    c.x /= nb_points;
    c.y /= nb_points;

    // 2. Initialiser les données de travail

    for (uint16_t i = 0; i < max_points; i++)
    {
        work_data[i].furthest_idx = -1;
        work_data[i].max_dist_sq = -1.0f;
    }

    // 3. Classer chaque point dans un secteur et garder le plus éloigné (Outer Bound)
    for (uint16_t i = 0; i < nb_points; i++)
    {
        distance_t dx = points[i].x - c.x;
        distance_t dy = points[i].y - c.y;

        // Calcul de l'angle (0 à 2*PI)
        angle_t angle = atan2f(dy, dx);
        if (angle < 0)
        {
            angle += 2.0f * PI;
        }

        // Déterminer l'index du secteur
        uint16_t s = (uint16_t)((angle / (2.0f * PI)) * max_points);
        if (s >= max_points)
        {
            s = max_points - 1;
        }

        float dist_sq = dx * dx + dy * dy;
        if (dist_sq > work_data[s].max_dist_sq)
        {
            work_data[s].max_dist_sq = dist_sq;
            work_data[s].furthest_idx = i;
        }
    }

    // 4. Remplir le polygone de sortie avec les points sélectionnés
    out_polygon->nb_points = 0;
    for (uint16_t i = 0; i < max_points; i++)
    {
        if (work_data[i].furthest_idx != -1)
        {
            out_polygon->points[out_polygon->nb_points++] = points[work_data[i].furthest_idx];
        }
    }

    return (out_polygon->nb_points >= 3);
}

/* ***************************************** Public callback functions definitions *************************************** */
