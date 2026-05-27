/**
 * @file dj_obstacle_manager.h
 * @brief Manager for all obstacles
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_obstacle_manager.c
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_config.h"
#include "utils/dj/dj_obstacle/dj_obstacle_dynamic.h"
#include "utils/dj/dj_obstacle/dj_obstacle_static.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_id.h"
#include "utils/dj/dj_obstacle_importer/dj_obstacle_importer.h"
#include "utils/dj/dj_viewer/dj_viewer_status.h"
#include "utils/lists/magic_array.h"
/* ***************************************************** Public macros *************************************************** */

/* ************************************************** Public types definition ******************************************** */

/**
 * @brief The obstacle manager object
 */
typedef struct
{
    /**
     * @brief The list of all obstacles
     * @warning This list is not always up to date, use dj_obstacle_manager_get_all_obstacles to get the list of all obstacles
     */
    static_obstacles_list_t computed_obstacles;
    /**
     * @brief The last viewer status used to compute the dynamic obstacles
     */
    dj_viewer_status_t last_computed_viewer;
    /**
     * @brief Flag to know if the last viewer status is valid
     */
    bool last_viewer_is_valid;
    /**
     * @brief Flag to know if the obstacles must be recomputed
     */
    bool must_recompute;
    /**
     * @brief Flag to know if the static obstacles are added
     */
    bool static_obstacles_added;
    /**
     * @brief Number of static obstacles added in the computed obstacles list
     */
    uint16_t nb_static_obstacles;
    /**
     * @brief If true, all dynamic obstacles have null speed and acceleration.
     * When set, get_all_obstacles skips dynamic recomputation after the first
     * valid computation, since obstacle positions are viewer-independent.
     */
    bool all_dynamic_null_kinematics;
    /**
     * @brief The obstacle importer to use
     */
    dj_obstacle_importer_t *obstacle_importer;
} dj_obstacle_manager_t;

/* *********************************************** Public functions declarations ***************************************** */

/**
 * @brief Initialize an obstacle manager object
 * @param manager The obstacle manager object to initialize
 * @param obstacle_importer The obstacle importer to use
 */
void dj_obstacle_manager_init(dj_obstacle_manager_t *manager,
                              dj_obstacle_importer_t *obstacle_importer);

/**
 * @brief Get all obstacles from the obstacle manager
 * @param manager The obstacle manager object
 * @param viewer_status The viewer status (NULL if you want only static obstacles)
 * @return obstacles_list_t* The list of all obstacles
 */
static_obstacles_list_t *dj_obstacle_manager_get_all_obstacles(dj_obstacle_manager_t *manager,
                                                               dj_viewer_status_t *viewer_status);

/**
 * @brief Function to clear all obstacles from the obstacle manager
 * @param manager The obstacle manager object
 */
void dj_obstacle_manager_clear_obstacles(dj_obstacle_manager_t *manager);

/**
 * @brief Function to check if a point is on an obstacle
 * @param obstacles The list of obstacles
 * @param point The point to check
 *
 * @return bool True if the node is on an obstacle, false otherwise
 */
bool dj_obstacle_manager_is_point_on_obstacle(static_obstacles_list_t *obstacles, point_t point);

/* ******************************************* Public callback functions declarations ************************************ */
