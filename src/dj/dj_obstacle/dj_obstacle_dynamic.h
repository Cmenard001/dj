/**
 * @file dj_obstacle_dynamic.h
 * @brief Dynamic obstacles are obstacles that can move in a straight line with a constant acceleration
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_dynamic.c
 */

#ifndef __DJ_OBSTACLE_DYNAMIC_H__
#define __DJ_OBSTACLE_DYNAMIC_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_obstacle_importer/dj_obstacle_id.h"
#include "../dj_viewer/dj_viewer_status.h"
#include "dj_obstacle.h"
#include "dj_polygon.h"

#include <stdint.h>

/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief Dynamic obstacle structure
 * @note A dynamic obstacle is an obstacle that can linearly move in a straight line
 * @note The obstacle has a constant acceleration so a linear speed
 * @note the obstacle has a max speed and a initial speed
 */
typedef struct
{
    DJ_OBSTACLE_MOTHER_ATTRIBUTES
    /**
     * @brief Shape of the obstacle
     * @warning The shape must be centralized as best as possible on the origin
     */
    dj_dynamic_obstacle_id_e m_id;       /**< ID of the obstacle */
    dj_polygon_t m_initial_shape;        /**< Shape of the obstacle */
    GEOMETRY_point_t m_initial_position; /**< Initial position of the obstacle */
    GEOMETRY_vector_t m_initial_speed;   /**< Initial speed of the obstacle */
    GEOMETRY_vector_t m_acceleration;    /**< Acceleration of the obstacle */
    time32_t m_update_time;              /**< Time of the last update */
} dj_obstacle_dynamic_t;

/**
 * @brief Structure to store the solutions of the dynamic obstacle
 * @note The solutions are the positions of the obstacle in space time from the viewer position
 * @note There is [0 ; 8] solution(s)
 */
typedef struct
{
    dj_polygon_t m_solutions[8]; /**< Positions of the obstacle */
    uint8_t m_nb_solutions;      /**< Number of positions of the obstacle */
} dj_dynamic_polygon_solution_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Function to initialize a dynamic obstacle
 *
 * @param obstacle Pointer to the obstacle to initialize
 * @param nb_points Number of points of the obstacle
 * @param get_position Function to get the position of the obstacle
 * @param initial_shape Shape of the obstacle
 * @param initial_position Initial position of the obstacle
 * @param initial_speed Initial speed of the obstacle
 * @param acceleration Acceleration of the obstacle
 * @param id ID of the obstacle (must be unique)
 * @param is_enabled true if the obstacle is enabled, false otherwise
 */
void dj_obstacle_dynamic_init(dj_obstacle_dynamic_t *obstacle,
                              dj_polygon_t *initial_shape,
                              GEOMETRY_point_t *initial_position,
                              GEOMETRY_vector_t *initial_speed,
                              GEOMETRY_vector_t *acceleration,
                              dj_dynamic_obstacle_id_e id,
                              bool is_enabled);

/**
 * @brief Function to deinitialize a dynamic obstacle
 *
 * @param obstacle Pointer to the obstacle to deinitialize
 */
void dj_obstacle_dynamic_deinit(dj_obstacle_dynamic_t *obstacle);

/**
 * @brief Function to refresh the position of a dynamic obstacle
 *
 * @param obstacle Pointer to the obstacle to refresh
 * @param acceleration New acceleration of the obstacle
 * @param initial_speed New initial speed of the obstacle
 * @param initial_position New initial position of the obstacle
 */
void dj_obstacle_dynamic_refresh(dj_obstacle_dynamic_t *obstacle,
                                 GEOMETRY_vector_t *acceleration,
                                 GEOMETRY_vector_t *initial_speed,
                                 GEOMETRY_point_t *initial_position);

/**
 * @brief Function to get the position of a dynamic obstacle in space time
 *
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

#endif
