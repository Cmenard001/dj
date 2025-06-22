/**
 * @file dj_config.h
 * @brief Configuration du module DJ
 * @author Cyprien Ménard
 * @date 26/03/2025
 * @see dj_config.c
 */

#ifndef __DJ_CONFIG_H__
#define __DJ_CONFIG_H__

/* ******************************************************* Includes ****************************************************** */

#include "dj_dependencies/dj_dependencies.h"

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Enable the DJ framework for the ESEO robot
 */
#define DJ_ROBOT_ESEO_FRAMEWORK 0

/**
 * @brief The size of the playground
 */
#define DJ_PLAYGROUND_X_SIZE (2000)
#define DJ_PLAYGROUND_Y_SIZE (3000)

/**
 * @brief
 * @brief Fictive exemple values
 */
#define DJ_ROBOT_WIDTH (324 + 95)
#define DJ_ROBOT_FRONT_BORDER_DISTANCE (130)
#define DJ_ROBOT_BACK_BORDER_DISTANCE (130)

/**
 * @brief The margin to add to the robot radius to check if a node is on the playground
 */
#define ROBOT_MARGIN (50)

/**
 * @brief Maximum number of nodes in the graph
 */
#define DJ_GRAPH_GRAPH_MAX_NODES (300)

/**
 * @brief Average number of links per node
 */
#define DJ_GRAPH_GRAPH_AVERAGE_LINKS (10)

/**
 * @brief Maximum number of point in a path
 */
#define DJ_GRAPH_GRAPH_MAX_PATH_LENGHT (100)

/**
 * @brief Maximum number of paths generated
 */
#define DJ_GRAPH_GRAPH_MAX_PATHS (DJ_GRAPH_GRAPH_MAX_NODES)

/**
 * @brief DJ enable/disable logs of dj
 */
#define DJ_ENABLE_DEBUG_LOGS 0
#define DJ_ENABLE_BUILD_GRAPH_DEBUG_LOGS 0 && DJ_ENABLE_DEBUG_LOGS
#define DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS 0 && DJ_ENABLE_DEBUG_LOGS

/**
 * @brief Enable / disable the optimization of the rebuild
 * @note The optimization consists in not adding the nodes of dynamic obstacles.
 *      The consequence is that the robot will go along the static obstacles only.
 */
#define DJ_ENABLE_REBUILD_OPTIMIZATION 1

/**
 * ENABLE_RECALCULATION
 * @brief Enable or disable the recalculation of the path during the displacement
 *
 * DJ_RECALCULATION_DISTANCE
 * @brief Distance between two path recalculation
 *
 * DJ_RECALCULATION_DISTANCE_FROM_NODE
 * @brief Distance from the last/next node to the recalculation point
 * @details This distance helps avoid recalculating the path too close to an obstacle:
 *          - In case of END_AT_BRAKE, the robot might slightly cross the obstacle.
 *          - If the recalculation point is too close to the obstacle, the path may cut through it.
 */
#define ENABLE_RECALCULATION 0
#if ENABLE_RECALCULATION
#define DJ_RECALCULATION_DISTANCE 500
#define DJ_RECALCULATION_DISTANCE_FROM_NODE 100
#endif

/**
 * @brief Enable or disable to retry the path generation if an avoidance is triggered
 *        + the number of retries
 */
#define ENABLE_RETRY 0
#if ENABLE_RETRY
#define MAX_RETRIES 2
#endif

/**
 * @brief DJ timeout
 */
#define DJ_TIMEOUT_MS 15000 // ms

/**
 * @brief Distance to consider that a displacement is too short
 */
#define TOO_SHORT_DISTANCE 40 // mm

/**
 * @brief Maximum life time of an dynamic obstacle
 */
#define OBSTACLE_LIFETIME_MS DJ_TIMEOUT_MS // (ms)

/**
 * @brief Maximum number of static obstacles wich are dynamically imported
 */
#define DJ_OBSTACLE_MANAGER_MAX_IMPORTED_OBSTACLES 5

/**
 * @brief Number max of points of a polygon
 */
#define DJ_POLYGON_MAX_POINTS 40

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */

#endif
