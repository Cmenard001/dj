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
 */

/* ******************************************************* Includes ****************************************************** */
#include "dj_oversize_obstacle.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_logs/dj_logs.h"
#include "../dj_prop_config/dj_prop_config.h"
#include "dj_polygon.h"

/* **************************************************** Private macros *************************************************** */

/**
 * @brief Oversize size
 * @note Unit : [mm]
 * @note This macro can be used if margin is defined
 */
#define DJ_OVERSIZE_SIZE (DJ_ROBOT_WIDTH / 2 + margin)

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
    dj_control_non_null(polygon, )

        // First, we need to know if the polygon is in trigonometric order
        int32_t area
        = 0;

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

/**
 * @brief Oversize a corner of the polygon
 * @details This function will add DJ_MAX_POINTS_PER_CORNER points in the output polygon
 * @param[in] original_polygon : the original polygon
 * @param[out] output_polygon : the polygon with the oversize
 * @param[in] index : the index of the point to oversize
 * @param[in] mode : oversize mode
 * @param[in] margin : margin to take into account in millimeters
 */
static void oversize_point(dj_polygon_t *original_polygon,
                           dj_polygon_t *output_polygon,
                           uint16_t index,
                           dj_obsrtacle_oversize_mode_e mode,
                           uint8_t margin)
{
    dj_control_non_null(original_polygon, ) dj_control_non_null(output_polygon, )

        GEOMETRY_point_t point
        = original_polygon->points[index];
    GEOMETRY_point_t next_point = original_polygon->points[(index + 1) % original_polygon->nb_points];
    GEOMETRY_point_t prev_point
        = original_polygon->points[(index + original_polygon->nb_points - 1) % original_polygon->nb_points];

    // Compute the angle of each segment
    int16_t angle_next = GEOMETRY_viewing_angle(point.x, point.y, next_point.x, next_point.y);
    int16_t angle_prev = GEOMETRY_viewing_angle(point.x, point.y, prev_point.x, prev_point.y);
    int16_t angle_total = GEOMETRY_modulo_angle(angle_next - angle_prev - PI4096);

    // Compute the number of points to add
    uint16_t nb_points = 0;
    if (angle_total < 0)
    {
        nb_points = 1;
        mode = DJ_OBSRTACLE_OVERSIZE_MODE_1_POINT;
    }
    else if (mode == DJ_OBSRTACLE_OVERSIZE_MODE_END_AT_BRAKE)
    {
#if MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD
        nb_points = angle_total / MIN_ANGLE_PRE_ROTATION_IMMOBILE_RAD + 1;
#else
        nb_points = 1;
#endif
    }
    else
    {
        nb_points = (uint16_t)mode;
    }

    // Compute between each point
    int16_t angle = GEOMETRY_modulo_angle(angle_total) / (nb_points);

    // Compute the first angle
    int16_t current_angle = GEOMETRY_modulo_angle(angle_prev + PI4096 / 2 + angle / 2);

    // Compute all the oversized points
    for (uint16_t point_index = 0; point_index < nb_points; point_index++)
    {
        // Compute the distance between the point and the new point
        int16_t distance = DJ_OVERSIZE_SIZE / cos4096(angle / 2);

        // Compute the new point
        GEOMETRY_point_t new_point;
        GEOMETRY_calculate_point_from_distance_and_angle(point, current_angle, distance, &new_point);

        // Add the point to the output polygon
        if (output_polygon->nb_points >= DJ_POLYGON_MAX_POINTS)
        {
            dj_debug_printf("dj polygon error: too many points\n");
            return;
        }
        output_polygon->points[output_polygon->nb_points] = new_point;
        output_polygon->nb_points++;

        // Update the current angle
        current_angle = GEOMETRY_modulo_angle(current_angle + angle);
    }
}

/* ********************************************** Public functions definitions ******************************************* */

void dj_oversize_obstacle(dj_polygon_t *polygon, dj_obsrtacle_oversize_mode_e mode, uint8_t margin)
{
    if (mode == DJ_OBSRTACLE_OVERSIZE_MODE_NO_OVERSIZE)
    {
        return;
    }
    // Make a copy of the original polygon
    dj_polygon_t original_polygon = *polygon;
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
        oversize_point(&original_polygon, &output_polygon, i, mode, margin);
    }

    // return the polygon with the oversize
    *polygon = output_polygon;
}

/* ***************************************** Public callback functions definitions *************************************** */
