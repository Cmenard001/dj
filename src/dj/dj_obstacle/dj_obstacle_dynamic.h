/**
 * @file dj_obstacle_dynamic.h
 * @brief Dynamic obstacles are obstacles that can move in a straight line with a constant acceleration
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_dynamic.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_obstacle/dj_obstacle.h"
#include "utils/dj/dj_obstacle/dj_polygon.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include "utils/maths/kinematics/linear_acceleration/linear_acceleration.h"
#include "utils/maths/kinematics/linear_speed/linear_speed.h"
#include "utils/time/time.h"
#include <stdint.h>
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Structure to store the solutions of the dynamic obstacle
 * @note The solutions are the positions of the obstacle in space time from the viewer position
 * @note There is [0 ; 8] solution(s)
 */
typedef struct
{
    dj_polygon_t solutions[8]; /**< Positions of the obstacle */
    uint8_t nb_solutions;      /**< Number of positions of the obstacle */
} dj_dynamic_polygon_solution_t;

/**
 * @brief Forward declaration of dj_obstacle_dynamic_t
 */
typedef struct dj_obstacle_dynamic_s dj_obstacle_dynamic_t;

/**
 * @brief Callback type to get the positions of a dynamic obstacle in space time
 * @param [in] obstacle Pointer to the dynamic obstacle
 * @param [in] viewer_status Viewer status
 * @param [out] solution Pointer to the solution to fill
 */
typedef void (*dj_obstacle_dynamic_get_position_cb_t)(dj_obstacle_dynamic_t *obstacle,
                                                      dj_viewer_status_t *viewer_status,
                                                      dj_dynamic_polygon_solution_t *solution);

/**
 * @brief Dynamic obstacle structure
 * @note A dynamic obstacle is an obstacle that can linearly move in a straight line
 * @note The obstacle has a constant acceleration so a linear speed
 * @note the obstacle has a max speed and a initial speed
 */
typedef struct dj_obstacle_dynamic_s
{
    DJ_OBSTACLE_MOTHER_ATTRIBUTES
    /**
     * @brief Shape of the obstacle
     * @warning The shape must be centralized as best as possible on the origin
     */
    dj_dynamic_obstacle_id_t id;                  /**< ID of the obstacle */
    dj_polygon_t initial_shape;                   /**< Shape of the obstacle */
    point_t initial_position;                     /**< Initial position of the obstacle */
    linear_speed_2d_vector_t initial_speed;       /**< Initial speed of the obstacle */
    linear_acceleration_2d_vector_t acceleration; /**< Acceleration of the obstacle */
    time_ms_t update_time;                        /**< Time of the last update */
    dj_obstacle_dynamic_get_position_cb_t
        get_position;        /**< Function to get the position of the obstacle */
    void *get_position_args; /**< Additional arguments for the get position function */
} dj_obstacle_dynamic_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a dynamic obstacle
 * @param obstacle Pointer to the obstacle to initialize
 * @param get_position Function to get the position of the obstacle
 * @param get_position_args Additional arguments for the get position function
 * @param initial_shape Shape of the obstacle
 * @param initial_position Initial position of the obstacle
 * @param initial_speed Initial speed of the obstacle
 * @param acceleration Acceleration of the obstacle
 * @param id ID of the obstacle (must be unique)
 * @param is_enabled true if the obstacle is enabled, false otherwise
 */
void dj_obstacle_dynamic_init(dj_obstacle_dynamic_t *obstacle,
                              dj_obstacle_dynamic_get_position_cb_t get_position,
                              void *get_position_args,
                              dj_polygon_t *initial_shape,
                              point_t *initial_position,
                              linear_speed_2d_vector_t *initial_speed,
                              linear_acceleration_2d_vector_t *acceleration,
                              dj_dynamic_obstacle_id_t id,
                              bool is_enabled);

/**
 * @brief Function to refresh the position of a dynamic obstacle
 * @param obstacle Pointer to the obstacle to refresh
 * @param shape New shape of the dynamic obstacle
 * @param acceleration New acceleration of the obstacle
 * @param initial_speed New initial speed of the obstacle
 * @param initial_position New initial position of the obstacle
 */
void dj_obstacle_dynamic_refresh(dj_obstacle_dynamic_t *obstacle,
                                 dj_polygon_t *shape,
                                 linear_acceleration_2d_vector_t *acceleration,
                                 linear_speed_2d_vector_t *initial_speed,
                                 point_t *initial_position);

/**
 * @brief Function to get the position of a dynamic obstacle in space time
 * @param [in] obstacle Pointer to the dynamic obstacle
 * @param [in] viewer_status Viewer status
 * @param [out] solution Pointer to the solution to fill
 *
 * @note There is [0 ; 8] solution(s)
 * @see dj_dynamic_polygon_solution_t for more information
 */
void dj_obstacle_dynamic_get_position(dj_obstacle_dynamic_t *obstacle,
                                      dj_viewer_status_t *viewer_status,
                                      dj_dynamic_polygon_solution_t *solution);

/* ******************************************* Public callback functions declarations ************************************ */
