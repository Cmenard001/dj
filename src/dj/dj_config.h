/**
 * @file dj_config.h
 * @brief Configuration du module DJ
 * @author Cyprien Ménard
 * @date 26/03/2025
 * @see dj_config.c
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

#pragma once
#include "utils/macros/macros.h"
/* ******************************************************* Includes ****************************************************** */

/* ***************************************************** Public macros *************************************************** */

/**
 * @brief Maximum number of nodes in the graph
 */
#define DJ_GRAPH_GRAPH_MAX_NODES       500

/**
 * @brief Average number of links per node
 */
#define DJ_GRAPH_GRAPH_AVERAGE_LINKS   10

/**
 * @brief Maximum number of point in a path
 */
#define DJ_GRAPH_GRAPH_MAX_PATH_LENGHT 100

/**
 * @brief Maximum number of paths generated
 */
#define DJ_GRAPH_GRAPH_MAX_PATHS       DJ_GRAPH_GRAPH_MAX_NODES

/**
 * @brief Enable / disable the optimization of the rebuild
 * @note The optimization consists in not adding the nodes of dynamic obstacles.
 *      The consequence is that the robot will go along the static obstacles only.
 */
#define DJ_ENABLE_REBUILD_OPTIMIZATION 0

/**
 * @brief Maximum number of destinations for multi-destination path generation
 */
#define DJ_ENGINE_MAX_DESTINATIONS     30

/**
 * @brief Maximum number of adjacent links per node
 */
#define DJ_GRAPH_NODE_MAX_ADJACENCY    DJ_GRAPH_GRAPH_MAX_NODES

/**
 * @brief Number max of points of a polygon
 */
#define DJ_POLYGON_MAX_POINTS          40

/* ************************************************** Public types definition ******************************************** */

/* *********************************************** Public functions declarations ***************************************** */

/* ******************************************* Public callback functions declarations ************************************ */
