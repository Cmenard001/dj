/**
 * @file dj_obstacle_dynamic.c
 * @brief Dynamic obstacles are obstacles that can move in a straight line with a constant acceleration
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_dynamic.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle_dynamic.h"
#include "system/assert/system_assert.h"
#include "utils/dj/dj_config.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include <math.h>
#include <stdlib.h>

/* **************************************************** Private macros *************************************************** */

/* ************************************************ Private type definition ********************************************** */

/* ********************************************* Private functions declarations ****************************************** */

/* ************************************************** Private variables ************************************************** */

/* ********************************************** Private functions definitions ****************************************** */

/* *********************************************** Public functions declarations ***************************************** */

void dj_obstacle_dynamic_init(dj_obstacle_dynamic_t *obstacle,
                              dj_obstacle_dynamic_get_position_cb_t get_position,
                              void *get_position_args,
                              dj_polygon_t *initial_shape,
                              point_t *initial_position,
                              linear_speed_2d_vector_t *initial_speed,
                              linear_acceleration_2d_vector_t *acceleration,
                              dj_dynamic_obstacle_id_t id,
                              bool is_enabled)
{
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(initial_shape != NULL);
    SYSTEM_ASSERT(initial_position != NULL);
    SYSTEM_ASSERT(initial_speed != NULL);
    SYSTEM_ASSERT(acceleration != NULL);
    SYSTEM_ASSERT(get_position != NULL);

    dj_obstacle_init((dj_obstacle_t *)obstacle, DJ_OBSTACLE_TYPE_DYNAMIC, is_enabled);
    obstacle->id = id;
    obstacle->initial_position = *initial_position;
    obstacle->initial_speed = *initial_speed;
    obstacle->acceleration = *acceleration;
    obstacle->update_time = 0;
    obstacle->initial_shape.nb_points = dj_polygon_get_nb_points(initial_shape);
    obstacle->get_position = get_position;
    obstacle->get_position_args = get_position_args;
    for (uint8_t i = 0; i < dj_polygon_get_nb_points(initial_shape); i++)
    {
        obstacle->initial_shape.points[i] = *dj_polygon_get_point(initial_shape, i);
    }
}

void dj_obstacle_dynamic_refresh(dj_obstacle_dynamic_t *obstacle,
                                 dj_polygon_t *shape,
                                 linear_acceleration_2d_vector_t *acceleration,
                                 linear_speed_2d_vector_t *initial_speed,
                                 point_t *initial_position)
{
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(acceleration != NULL);
    SYSTEM_ASSERT(initial_speed != NULL);
    SYSTEM_ASSERT(initial_position != NULL);
    SYSTEM_ASSERT(shape != NULL);
    obstacle->acceleration = *acceleration;
    obstacle->initial_shape = *shape;
    obstacle->initial_speed = *initial_speed;
    obstacle->initial_position = *initial_position;
    obstacle->update_time = time_get_absolute_time_ms();
}

void dj_obstacle_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                      dj_viewer_status_t *viewer_status,
                                      dj_dynamic_polygon_solution_t *solution)
{
    SYSTEM_ASSERT(obstacle != NULL);
    SYSTEM_ASSERT(viewer_status != NULL);
    SYSTEM_ASSERT(solution != NULL);
    SYSTEM_ASSERT(obstacle->get_position != NULL);

    obstacle->get_position(obstacle, viewer_status, solution);
}

/* ******************************************* Public callback functions declarations ************************************ */
