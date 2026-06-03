/**
 * @file dj_oversize_obstacle.c
 * @brief Tool to oversize an obstacle
 * Why do we need to oversize an obstacle?
 * User define an obstacle which in all the area of robot mustn't go through.
 * But when we make displacement, we consider the center of the robot.
 * So, we need to oversize the obstacle by half of the robot's width.
 * @author Cyprien Ménard
 * @date 04/02/2025
 * @see dj_oversize_obstacle.h
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */
#include "utils/dj/dj_obstacle/dj_oversize_obstacle.h"
#include "propulsion/copilot/copilot_order/copilot_order.h"
#include "system/assert/system_assert.h"
#include "system/log/log.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/maths/angle/angle.h"
#include "utils/maths/position/position.h"
#include <math.h>
/* **************************************************** Private macros *************************************************** */

LOG_REGISTER("utils/dj/oversize_obstacle");

/**
 * @brief Maximum angle between two points when oversizing a corner as an end at brake
 */
#define DJ_OBSTACLE_OVERSIZE_MAX_ANGLE_RAD (CPLT_TRAJ_MAX_ANGLE_DIFF_FOR_CONTINUOUS_TRAJ_RAD)

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/**
 * @brief Set the polygon in trigonometric order
 * @details This function will reverse the order of the points if the polygon is in hourly order
 * @param[in] polygon : the polygon to set in trigonometric order
 */
static void dj_polygon_set_trigonometric_order(dj_polygon_t *polygon)
{
    SYSTEM_ASSERT(polygon != NULL);

    // First, we need to know if the polygon is in trigonometric order
    float area = 0.0f;

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

/**
 * @brief Oversize a corner of the polygon
 * @details This function will add DJ_MAX_POINTS_PER_CORNER points in the output polygon
 * @param[in] original_polygon : the original polygon
 * @param[out] output_polygon : the polygon with the oversize
 * @param[in] index : the index of the point to oversize
 * @param[in] mode : oversize mode
 * @param[in] oversize_distance : distance to take into account in millimeters
 */
static void oversize_point(dj_polygon_t *original_polygon,
                           dj_polygon_t *output_polygon,
                           uint16_t index,
                           dj_obstacle_oversize_mode_t mode,
                           distance_t oversize_distance)
{
    SYSTEM_ASSERT(original_polygon != NULL);
    SYSTEM_ASSERT(output_polygon != NULL);

    point_t point = original_polygon->points[index];
    point_t next_point = original_polygon->points[(index + 1) % original_polygon->nb_points];
    point_t prev_point =
        original_polygon
            ->points[(index + original_polygon->nb_points - 1) % original_polygon->nb_points];

    // Compute the angle of each segment
    angle_t angle_next, angle_prev;
    viewing_angle(&point, &next_point, &angle_next);
    viewing_angle(&point, &prev_point, &angle_prev);

    // Compute the angle between the two segments
    // We add 2*PI to avoid negative angles
    // Do not use modulo_angle here because we want the angle to be between 0 and 2*PI
    // This value is positive due to trigonometric order of the polygon
    angle_t angle_total = angle_next - angle_prev - PI;
    while (angle_total < 0)
    {
        angle_total += 2.0f * PI;
    }

    // Compute the number of points to add
    uint16_t nb_points = 0;
    if (mode == DJ_OBSTACLE_OVERSIZE_MODE_END_AT_BRAKE)
    {
        // Compute the number of points based on the maximum angle
        // Add 1 to round up
        nb_points = (uint16_t)(angle_total / DJ_OBSTACLE_OVERSIZE_MAX_ANGLE_RAD) + 1;
    }
    else
    {
        nb_points = (uint16_t)mode;
    }

    SYSTEM_ASSERT(nb_points >= 1);

    // Else, we have multiple points to add

    // Compute between each point (positive value)
    angle_t angle = angle_total / (angle_t)(nb_points);

    // Compute the first angle
    angle_t current_angle = modulo_angle(angle_prev + PI / 2.0f + angle / 2.0f);

    // Compute the distance between the point and the new point
    distance_t distance = oversize_distance / cosf(angle / 2.0f);

    // Compute all the oversized points
    for (uint16_t point_index = 0; point_index < nb_points; point_index++)
    {
        // Compute the new point using position and angle
        position_t pos = {.point = point, .angle = current_angle};
        point_t new_point;
        calculate_point_from_distance_and_position(&pos, distance, &new_point);

        // Add the point to the output polygon
        if (output_polygon->nb_points >= DJ_POLYGON_MAX_POINTS)
        {
            LOGE("Too many points");
            return;
        }
        output_polygon->points[output_polygon->nb_points] = new_point;
        output_polygon->nb_points++;

        // Update the current angle
        current_angle = modulo_angle(current_angle + angle);
    }
}

/* ********************************************** Public functions definitions ******************************************* */

void dj_oversize_obstacle(dj_polygon_t *polygon,
                          dj_obstacle_oversize_mode_t mode,
                          distance_t oversize_distance)
{
    if (mode == DJ_OBSTACLE_OVERSIZE_MODE_NO_OVERSIZE)
    {
        return;
    }
    // Make a copy of the original polygon
    dj_polygon_t original_polygon;
    dj_polygon_copy(&original_polygon, polygon);
    // Output polygon
    // At the beginning, the output polygon is empty
    // The output polygon will be filled with
    // the oversize points and the size will be increased
    dj_polygon_t output_polygon = {.nb_points = 0};

    // Make sure the input polygon is set in trigonometric order
    dj_polygon_set_trigonometric_order(&original_polygon);

    // Make an oversize of each point of the polygon
    for (uint16_t i = 0; i < original_polygon.nb_points; i++)
    {
        // oversize the point
        oversize_point(&original_polygon, &output_polygon, i, mode, oversize_distance);
    }

    // return the polygon with the oversize
    dj_polygon_copy(polygon, &output_polygon);
}

/* ***************************************** Public callback functions definitions *************************************** */
