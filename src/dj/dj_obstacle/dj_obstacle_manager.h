/**
 * @file dj_obstacle_manager.h
 * @brief Manager for all obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_manager.c
 */

#ifndef __DJ_OBSTACLE_MANAGER_H__
#define __DJ_OBSTACLE_MANAGER_H__

/* ******************************************************* Includes ****************************************************** */

#include "../dj_config.h"
#include "../dj_dependencies/dj_dependencies.h"
#include "../dj_obstacle_importer/dj_obstacle_id.h"
#include "../dj_viewer/dj_viewer_status.h"
#include "dj_obstacle_dynamic.h"
#include "dj_obstacle_static.h"

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of computed obstacles
 */
#define DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES (DJ_OBSTACLE_MANAGER_MAX_IMPORTED_OBSTACLES + STATIC_OBSTACLE_COUNT)

/* ************************************************** Public types definition ******************************************** */

MAGIC_ARRAY_TYPEDEF(static_obstacles_list, dj_obstacle_static_t, DJ_OBSTACLE_MANAGER_MAX_STATIC_OBSTACLES);

/**
 * @brief The obstacle manager object
 */
typedef struct
{
    /**
     * @brief The list of all obstacles
     * @warning This list is not always up to date, use dj_obstacle_manager_get_all_obstacles to get the list of all obstacles
     */
    static_obstacles_list_t m_computed_obstacles;
    /**
     * @brief The last viewer status used to compute the dynamic obstacles
     */
    dj_viewer_status_t m_last_computed_viewer;
    /**
     * @brief Flag to know if the last viewer status is valid
     */
    bool m_last_viewer_is_valid;
    /**
     * @brief Flag to know if the obstacles must be recomputed
     */
    bool m_must_recompute;
    /**
     * @brief Flag to know if the static obstacles are added
     */
    bool m_static_obstacles_added;
    /**
     * @brief Number of static obstacles added in the computed obstacles list
     */
    uint16_t m_nb_static_obstacles;
} dj_obstacle_manager_t;

/* *********************************************** Public functions declarations ***************************************** */

MAGIC_ARRAY_HEADER(static_obstacles_list, dj_obstacle_static_t);

/**
 * @brief Initialize an obstacle manager object
 *
 * @param manager The obstacle manager object to initialize
 */
void dj_obstacle_manager_init(dj_obstacle_manager_t *manager);

/**
 * @brief Deinitialize an obstacle manager object
 *
 * @param manager The obstacle manager object to deinitialize
 */
void dj_obstacle_manager_deinit(dj_obstacle_manager_t *manager);

/**
 * @brief Get all obstacles from the obstacle manager
 *
 * @param manager The obstacle manager object
 * @param viewer_status The viewer status (NULL if you want only static obstacles)
 * @return obstacles_list_t* The list of all obstacles
 */
static_obstacles_list_t *dj_obstacle_manager_get_all_obstacles(dj_obstacle_manager_t *manager,
                                                               dj_viewer_status_t *viewer_status);

/**
 * @brief Function to clear all obstacles from the obstacle manager
 *
 * @param manager The obstacle manager object
 */
void dj_obstacle_manager_clear_obstacles(dj_obstacle_manager_t *manager);

/**
 * @brief Function to check if a point is on an obstacle
 *
 * @param obstacles The list of obstacles
 * @param point The point to check
 *
 * @return bool True if the node is on an obstacle, false otherwise
 */
bool dj_obstacle_manager_is_point_on_obstacle(static_obstacles_list_t *obstacles, GEOMETRY_point_t point);

/* ******************************************* Public callback functions declarations ************************************ */

#endif
